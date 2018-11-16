/*
  Copyright 2018 Dean Camera (dean [at] fourwalledcubicle [dot] com)
  Copyright 2018 Alfredo Mazzinghi

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

/** \file
 *  \brief Keyboard tester board LED Hardware Driver (Template)
 *  This stub is for the board-specific component of the LUFA LEDs driver,
 *  for the LEDs (up to four) mounted on most development boards.
*/

#ifndef __LEDS_USER_H__
#define __LEDS_USER_H__

#include <avr/io.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(__INCLUDE_FROM_LEDS_H)
#error Do not include this file directly. Include LUFA/Drivers/Board/LEDS.h instead.
#endif

/** LED mask for the first LED on the board. pin 21*/
#define LEDS_LED1        (1 << 3)

/** LED mask for the second LED on the board. pin 22 */
#define LEDS_LED2        (1 << 5)

/** LED mask for the third LED on the board. */
#define LEDS_LED3        0

/** LED mask for the fourth LED on the board. */
#define LEDS_LED4        0

/** LED mask for all the LEDs on the board. */
#define LEDS_ALL_LEDS    (LEDS_LED1 | LEDS_LED2 | LEDS_LED3 | LEDS_LED4)

/** LED mask for none of the board LEDs. */
#define LEDS_NO_LEDS     0

#if !defined(__DOXYGEN__)
static inline void LEDs_Init(void)
{
	/* Set output direction */
	DDRD |= (LEDS_LED1 | LEDS_LED2);
	/* Shut off leds */
	PORTD &= ~(LEDS_LED1 | LEDS_LED2);
}

static inline void LEDs_Disable(void)
{
	/* Reset pins to hi-z */
	DDRD &= ~(LEDS_LED1 | LEDS_LED2);
	/* Shut off leds */
	PORTD &= ~(LEDS_LED1 | LEDS_LED2);
}

static inline void LEDs_TurnOnLEDs(const uint8_t LEDMask)
{
	PORTD |= (LEDMask & LEDS_ALL_LEDS);
}

static inline void LEDs_TurnOffLEDs(const uint8_t LEDMask)
{
	PORTD &= ~(LEDMask & LEDS_ALL_LEDS);
}

static inline void LEDs_SetAllLEDs(const uint8_t LEDMask)
{
	LEDs_TurnOnLEDs(LEDMask);
	LEDs_TurnOffLEDs(~LEDMask);
}

/* 
 * Set the Leds in the given LEDMask to the status given in ActiveMask here
 */
static inline void LEDs_ChangeLEDs(const uint8_t LEDMask, const uint8_t ActiveMask)
{
  	uint8_t safeLEDMask = LEDMask & LEDS_ALL_LEDS;
	uint8_t safeActiveMask = ActiveMask & LEDS_ALL_LEDS;
	PORTD = (PORTD & ~safeLEDMask) | (safeLEDMask & safeActiveMask);
}

/*
 * Toggle the Leds in the given LEDMask, ignoring all others
 */
static inline void LEDs_ToggleLEDs(const uint8_t LEDMask)
{
	PORTD = (PORTD & ~LEDS_ALL_LEDS) | (~(PORTD & LEDMask) & LEDS_ALL_LEDS);
}

/*
 * Return the current LEDs status' here which can be masked against LED_LED* macros
 */
static inline uint8_t LEDs_GetLEDs(void) ATTR_WARN_UNUSED_RESULT;
static inline uint8_t LEDs_GetLEDs(void)
{
	return (PORTD & LEDS_ALL_LEDS);
}
#endif

#if defined(__cplusplus)
}
#endif

#endif

