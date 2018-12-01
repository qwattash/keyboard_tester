/*
  Copyright 2018  Alfredo Mazzinghi

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
#include "bitset.h"

static void matrixSelectColumn(int idx);
static void matrixClearColumn(int idx);
static bool matrixFetchRow(int idx);
static void matrixKeyPress(int row, int column);

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
		fprintf(&serialStream, "Button [%d, %d] pressed\r\n", row, column);
	BITSET_SET(lastKeystate, RC2IDX(row, column));
}

void
matrixReset()
{
	BITSET_CLEAR(lastKeystate);
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
		}
		matrixClearColumn(col);
	}
}

bool
matrixFillKeyboardReport()
{
	int idx, value;
	uint8_t scanCode;
	uint8_t nextKeycode = 0;

	BITSET_FOREACH(idx, value, lastKeystate) {
		if (!value)
			continue;
		// scanCode = layoutFetchScanCode(IDX2R(idx), IDX2C(idx));
		// keyboardReport->KeyCode[nextKeycode++] = scanCode;
		/* if (nextKeycode == MAX_REPORT_KEYCODES) */
		/* 	break; */
	}

	BITSET_CLEAR(lastKeystate);
	return true;
}
