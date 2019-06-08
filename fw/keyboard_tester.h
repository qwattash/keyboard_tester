/*
             LUFA Library
     Copyright (C) Dean Camera, 2018.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2019  Alfredo Mazzinghi
  Copyright 2018  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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

#ifndef _KEYBOARDTESTER_H_
#define _KEYBOARDTESTER_H_

#include <stdio.h>

/** 
 * LED mask for the library LED driver, to indicate that the
 * USB interface is not ready.
 */
#define LEDMASK_USB_NOTREADY      LEDS_NO_LEDS

/**
 * LED mask for the library LED driver, to indicate that the
 * USB interface is enumerating.
 */
#define LEDMASK_USB_ENUMERATING  LEDS_LED1

/**
 * LED mask for the library LED driver, to indicate that the
 * USB interface is ready.
 */
#define LEDMASK_USB_READY        LEDS_LED2

/**
 * LED mask for the library LED driver, to indicate that an
 * error has occurred in the USB interface.
 */
#define LEDMASK_USB_ERROR        (LEDS_LED1 | LEDS_LED2)

/**
 * Interval in ms between each keyboard matrix scan
 */
#define KEYBOARD_SCAN_INTERVAL_MS 5

/**
 * Number of ticks for ms of TIMER 1 with clk/64 prescaler
 */
#define TIMER1_TICKS 125

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

extern FILE serialStream;
extern bool hostConnected;
extern bool debugConnected;

#define DEBUG(fmt, ...) do {						\
		if (debugConnected)					\
			fprintf(&serialStream, fmt, ## __VA_ARGS__);	\
	} while (0)

#endif
