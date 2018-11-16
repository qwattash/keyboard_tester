/*
  Copyright 2018  Alfredo Mazzinghi
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

/**
 *
 *  Main source file for the USB device application. This file contains the
 *  main tasks of the application and is responsible for the initial
 *  application hardware configuration.
 */

#include "device.h"

/**
 * Main program entry point. This routine contains the overall program flow, including
 * setup of all components and the main program loop.
 */
int main(void)
{
	SetupHardware();

	GlobalInterruptEnable();

	for (;;)
	{
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization, atmega32u4 can only be USB_MODE_Device */
	USB_Init(USB_DEVICE_OPT_FULLSPEED | USB_OPT_AUTO_PLL);
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{

}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{

}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{

}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{

}
