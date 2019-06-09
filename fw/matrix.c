/*
  Copyright 2018, 2019  Alfredo Mazzinghi

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#include <stdbool.h>

#include <avr/cpufunc.h>
#include <avr/io.h>

#include "keyboard_tester.h"
#include "matrix.h"
#include "backlight.h"
#include "bitset.h"
#include "error.h"
#include "time.h"

bool ledChecked = false;

static void matrixSelectColumn(int idx);
static void matrixClearColumn(int idx);
static bool matrixFetchRow(int idx);
static void matrixKeyPress(int row, int column);
static void matrix_key_action(int row, int column);

typedef void (*timer_callback_t)(void);

/**
 * Current led selected.
 * 0 means no led selected.
 */
struct ledSelection {
	int row;
	int col;
	struct LedColor color;
	struct IS3733_State *state;
	int half;
};

/*
 * Current selected led. row=2 is no led selected.
 */
struct ledSelection currentLed = {2, 0};

/**
 * Backlight driver state
 */
static struct IS3733_State backlight_state;
/** Callback invoked by the backlight timer */
static timer_callback_t callback;

static void backlight_timer_set(uint32_t nanosec, timer_callback_t cbk);
static void backlight_do_check(void);
static void rotate_selected_led(struct IS3733_State *state);
static void breathe_selected_led(struct IS3733_State *state);
static void breathe_all_led(struct IS3733_State *state);
static void breathe_step(void);
static void breathe_all_step(void);
static bool backlight_color_step(void);

BITSET_DECLARE(KeystateBitset, KEYBOARD_ROWS * KEYBOARD_COLUMNS);

static int columnBits[] = {PF0, PF1, PF4};
static int rowBits[] = {PF5, PF6};
static KeystateBitset lastKeystate;

static void
matrixSelectColumn(int idx)
{
	int shift = columnBits[idx];
	PORTF |= (1 << shift);
}

static void
matrixClearColumn(int idx)
{
	int shift = columnBits[idx];
	PORTF &= ~(1 << shift);
}

static bool
matrixFetchRow(int idx)
{
	int shift = rowBits[idx];
	uint8_t row = (PINF & (1 << shift));

	return (row != 0);
}

static void
matrixKeyPress(int row, int column)
{
	if (BITSET_GET(lastKeystate, RC2IDX(row, column)) == 0)
		DEBUG("Button [%d, %d] pressed\r\n", row, column);
	BITSET_SET(lastKeystate, RC2IDX(row, column));
}

static void
matrixKeyRelease(int row, int column)
{
	if (BITSET_GET(lastKeystate, RC2IDX(row, column)) == 1) {
		DEBUG("Button [%d, %d] released\r\n", row, column);
		matrix_key_action(row, column);
	}
	BITSET_CLEAR(lastKeystate, RC2IDX(row, column));
}

void
matrixReset()
{
	BITSET_CLEAR_ALL(lastKeystate);
	for (int col = 0; col < KEYBOARD_COLUMNS; col++)
		matrixClearColumn(col);
}

void
matrixScan()
{
	for (int col = 0; col < KEYBOARD_COLUMNS; col++) {
		matrixSelectColumn(col);
		// latch delay for column signal propagation to row pins.
		_NOP();
		for (int row = 0; row < KEYBOARD_ROWS; row++) {
			if (matrixFetchRow(row))
				matrixKeyPress(row, col);
			else
				matrixKeyRelease(row, col);
		}
		matrixClearColumn(col);
	}
}

static uint8_t scanCodes[] = {
	HID_KEYBOARD_SC_A,
	HID_KEYBOARD_SC_B,
	HID_KEYBOARD_SC_C,
	HID_KEYBOARD_SC_D,
	HID_KEYBOARD_SC_E,
	HID_KEYBOARD_SC_F
};

bool
matrixFillKeyboardReport(USB_KeyboardReport_Data_t *keyboardReport)
{
	int idx, value;
	uint8_t scanCode;
	uint8_t nextKeycode = 0;

	BITSET_FOREACH(idx, value, lastKeystate) {
		if (!value)
			continue;
		// scanCode = layoutFetchScanCode(IDX2R(idx), IDX2C(idx));
		scanCode = scanCodes[idx];
		if (nextKeycode == 6) {
			DEBUG("Error: Key rollover - too many keys pressed %d\r\n",
			      nextKeycode);
			break;
		}
		keyboardReport->KeyCode[nextKeycode++] = scanCode;
	}

	return true;
}

static void
matrix_key_action(int row, int column)
{
	int idx = RC2IDX(row, column);

	switch(idx) {
	case 0:
		if (!ledChecked) {
			DEBUG("Trigger LED check\r\n");
			ledChecked = false;
			currentLed.row = 2;
			/* Initialize backlight */
			backlight_reset(&backlight_state);

			/* Start LED diagnostic */
			backlight_check_trigger(&backlight_state);
			backlight_timer_set(1000000, &backlight_do_check);
		}
		else {
			breathe_all_led(&backlight_state);
		}
		break;
	case 1:
		/* Missing key */
	case 2:
		if (ledChecked)
			backlight_set_pattern(&backlight_state);
		break;
	case 3:
		if (ledChecked) {
			rotate_selected_led(&backlight_state);
		}
		break;
	case 4:
		if (ledChecked) {
			breathe_selected_led(&backlight_state);
		}
		break;
	case 5:
		if (ledChecked)
			backlight_brightness(&backlight_state, 0);
		break;
	default:
		DEBUG("Error: invalid index");
	}
}

/**
 * Start breathe animation on selected led.
 * This will go through all possible colors in 0.10s intervals.
 */
static void
breathe_selected_led(struct IS3733_State *state)
{
	DEBUG("Breathe led [%d, %d]\r\n", currentLed.row, currentLed.col);
	currentLed.state = state;
	currentLed.color.r = 0;
	currentLed.color.g = 0;
	currentLed.color.b = 0;
	backlight_set(state, currentLed.row, currentLed.col, currentLed.color);
	backlight_timer_set(100000, &breathe_step);
}

static void
breathe_all_led(struct IS3733_State *state)
{
	backlight_brightness(state, 255);
	currentLed.state = state;
	currentLed.color.r = 0;
	currentLed.color.g = 0;
	currentLed.color.b = 0;
	currentLed.half = 0;
	backlight_set_all(state, 0, black);
	backlight_set_all(state, 1, black);
	backlight_timer_set(100000, &breathe_all_step);
}

static bool
backlight_color_step()
{
	uint8_t or = currentLed.color.r;
	uint8_t og = currentLed.color.g;
	uint8_t ob = currentLed.color.b;

	currentLed.color.r += 0x20;
	/* If wrap around, increment green */
	if (currentLed.color.r <= or) {
		currentLed.color.g += 0x20;
		/* If wrap around, increment blue */
		if (currentLed.color.g <= og) {
			currentLed.color.b += 0x20;
		}
	}

	DEBUG("Breathe step [%x, %x, %x]\r\n", currentLed.color.r,
	      currentLed.color.g, currentLed.color.b);
	if (ob <= currentLed.color.b)
		return true;
	return false;
}

static void
breathe_step()
{
	bool repeat = backlight_color_step();

	backlight_set(currentLed.state, currentLed.row, currentLed.col,
		      currentLed.color);
	/* When the blue wraps around we finish */
	if (repeat)
		backlight_timer_set(100000, &breathe_step);
}

static void
breathe_all_step()
{
	bool repeat = backlight_color_step();

	backlight_set_all(currentLed.state, currentLed.half, currentLed.color);
	/* When the blue wraps around we finish */
	if (repeat)
		backlight_timer_set(100000, &breathe_all_step);
	else if (currentLed.half == 0) {
		currentLed.color.r = 0;
		currentLed.color.g = 0;
		currentLed.color.b = 0;
		backlight_set_all(currentLed.state, 0, black);
		currentLed.half = 1;
		backlight_timer_set(100000, &breathe_all_step);
	}
}

/**
 * Rotate the currently selected LED.
 * Assume 10 keys in the keyboard.
 */
static void
rotate_selected_led(struct IS3733_State *state)
{
	DEBUG("Rotate led (%d, %d)\r\n", currentLed.row, currentLed.col);
	backlight_brightness(state, 255);

	if (currentLed.row != 2) {
		/* Switch off current led. */
		backlight_set(state, currentLed.row, currentLed.col, black);
	}

	/* Rotate led. */
	if (currentLed.row == 0) {
		/* Skip missing key */
		if (currentLed.col == 0 || currentLed.col == 3)
			currentLed.col += 2;
		else if (currentLed.col == 5) {
			currentLed.row += 1;
			currentLed.col = 0;
		}
		else
			currentLed.col += 1;
	}
	else if (currentLed.row == 1) {

		if (currentLed.col == 5) {
			currentLed.row += 1;
			currentLed.col = 0;
		}
		else
			currentLed.col += 1;
	}
	else {
		currentLed.row = 0;
		currentLed.col = 0;
	}

	DEBUG("Rotate led next (%d, %d)\r\n", currentLed.row, currentLed.col);
	/* If we switched to something valid, light it up. */
	if (currentLed.row != 2)
		backlight_set(state, currentLed.row, currentLed.col, bright_white);
}

/**
 * Initialize the backlight blinking and open-short detection.
 * We use timer 2 for this.
 */
void
init_backlight_timer()
{
	/* Select operation mode for timer 3
	 * We use CTC (Clear Timer on Compare match) WGM (waveform generation mode).
	 * Outputs OC3{A,B,C} of the waveform function generator are disconnected.
	 * We use the prescaler clock source with clk/64 division,
	 * we use channel A for compare and trigger an interrupt on match
	 */

	/* enable clock to timer 3 */
	PRR1 &= ~(1 << PRTIM3);

	// reset to normal mode for all channels (A, B, C), WGM3[1:0] = 0
	TCCR3A = 0;
	/* WGM3[3:2] select CTC */
	TCCR3B |= (1 << WGM32) | (0 << WGM33);

	TIFR3 = 0; // clear timer 3 interrupt flag register
	TIFR3 |= (1 << OCF3A); // enable OCF3A OC3A interrupt

	TIMSK3 = 0; // clear interrupt mask for timer 3

	/* Init the backlight subsystem */
	backlight_init(&backlight_state, I2C_BACKLIGHT_BUSADDR);
}

static void
backlight_do_check()
{
	DEBUG("LED check triggered\r\n");
	backlight_check(&backlight_state);
	ledChecked = true;
}

/**
 * Set the next backlight timer interval.
 */
static void
backlight_timer_set(uint32_t nanosec, timer_callback_t cbk)
{
	uint8_t clksource = get_timer3_clocksource(nanosec);
	uint16_t ticks = get_timer3_ticks(clksource, nanosec);

	DEBUG("Set backlight timer to %ldns ticks=%hd clksrc=%hhx\r\n", nanosec, ticks, clksource);

	/* set output compare registers */
	OCR3A = ticks;

	callback = cbk;

	TIMSK3 |= (1 << OCIE3A); // unmask OC3A interrupt

	/* Select clock source and start the timer, clk/64 prescaler */
	TCCR3B |= clksource;
}

ISR(TIMER3_COMPA_vect)
{
	/* Stop the timer */
	TIMSK3 = 0;
	TCCR3B = 0;

	if (callback)
		callback();
}
