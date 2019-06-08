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

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <LUFA/Drivers/USB/USB.h>

#include "backlight.h"

/**
 * Number of keyboard columns
 */
#define KEYBOARD_COLUMNS 3

/**
 * Number of keyboard rows
 */
#define KEYBOARD_ROWS 2

/**
 * Row-column to index conversion macros
 */
#define RC2IDX(row, col) (row * KEYBOARD_COLUMNS + col)
#define IDX2R(index) (index / KEYBOARD_COLUMNS)
#define IDX2C(index) (index % KEYBOARD_COLUMNS)

/**
 * Trigger a scan of the keyboard matrix
 */
void matrixScan(void);

/**
 * Reset the keyboard matrix state
 */
void matrixReset(void);

/**
 * Fill the given keyboard report with the last keys reported by
 * the scan loop.
 */
bool matrixFillKeyboardReport(USB_KeyboardReport_Data_t *HIDReport);

/**
 * Init the keyboard matrix backlight timer.
 */
void init_backlight_timer(void);

#endif /* _MATRIX_H_ */
