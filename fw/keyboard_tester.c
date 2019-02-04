/*
  Copyright 2018  Dean Camera (dean [at] fourwalledcubicle [dot] com)
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

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

#include "descriptors.h"
#include "keyboard_tester.h"
#include "matrix.h"

static void setupHardware(void);
static void initKeyboardScan(void);
static void deinitKeyboardScan(void);
static void startKeyboardScan(void);
static void stopKeyboardScan(void);

/**
 * Standard file stream for the CDC interface when set up,
 * so that the virtual CDC COM port can be
 * used like any regular character stream in the C APIs.
 */
FILE serialStream;
bool hostConnected = false;

static char banner[] = "Welcome to the KeyboardTester board DEBUG serial\r\n";

/** 
 * LUFA CDC Class driver interface configuration and state information.
 * This structure is passed to all CDC Class driver functions,
 * so that multiple instances of the same class within a device can
 * be differentiated from one another.
 */
static USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface = {
	.Config = {
		.ControlInterfaceNumber = INTERFACE_ID_CDC_CCI,
		.DataINEndpoint = {
			.Address = CDC_TX_EPADDR,
			.Size = CDC_TXRX_EPSIZE,
			.Banks = 1,
		},
		.DataOUTEndpoint = {
			.Address = CDC_RX_EPADDR,
			.Size = CDC_TXRX_EPSIZE,
			.Banks = 1,
		},
		.NotificationEndpoint = {
			.Address = CDC_NOTIFICATION_EPADDR,
			.Size = CDC_NOTIFICATION_EPSIZE,
			.Banks = 1,
		},
	},
};

/**
 * Buffer holding the previous HID Keyboard Report for internal
 * LUFA driver functionality purposes.
 */
static uint8_t prevHIDKeyboardReportBuffer[sizeof(USB_KeyboardReport_Data_t)];

/**
 * LUFA HID Keyboard Class driver interface configuration and state information.
 * This structure is passed to all HID Keyboard Class driver functions,
 * so that multiple instances of the same class within a device can be
 * differentiated from one another.
 */
static USB_ClassInfo_HID_Device_t Keyboard_HID_Interface = {
	.Config = {
		.InterfaceNumber = INTERFACE_ID_Keyboard,
		.ReportINEndpoint = {
			.Address = HID_REPORT_IN_EPADDR,
			.Size = HID_REPORT_EPSIZE,
			.Banks = 1
		},
		.PrevReportINBuffer = prevHIDKeyboardReportBuffer,
		.PrevReportINBufferSize = sizeof(prevHIDKeyboardReportBuffer)
	},
};

/**
 * Initialize the hardware
 */
static void
setupHardware()
{
	/* Disable watchdog */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Set port F mode to general purpose I/O
	 * output pin: 0,1,4 -- no pullup
	 * input pin: 5,6
	 */
	PORTF = 0;
	DDRF = (1 << DDF0) | (1 << DDF1) | (1 << DDF4);

	LEDs_Init();
	/* Hardware Initialization */
	USB_Init(USB_DEVICE_OPT_FULLSPEED | USB_OPT_AUTO_PLL);

	hostConnected = false;
}

/* 
 * Register interrupt handler for OC1A interrupt, which will be
 * triggered by the timer.
 */
ISR(TIMER1_COMPA_vect)
{
	/* if (hostConnected) { */
		matrixScan();
	/* } */
}

/**
 * Initialize keyboard matrix scan timer
 */
static void
initKeyboardScan()
{
	/* enable clock to timer 1 */
	PRR0 &= ~(1 << PRTIM1);

	// reset to normal mode for all channels (A, B, C), WGM1[1:0] = 0
	TCCR1A = 0;
	// reset control register B
	TCCR1B = 0;
	
	TIFR1 = 0; // clear timer 1 interrupt flag register
	TIMSK1 = 0; // clear interrupt mask for timer 1

	matrixReset();
}

/**
 * Shutdown keyboard matrix scan timer
 */
static void
deinitKeyboardScan()
{
	TIFR1 = 0; // clear timer 1 interrupt flag register
	TIMSK1 = 0; // clear interrupt mask for timer 1

	TCCR1A = 0; // reset to normal mode for all channels (A, B, C), WGM1[1:0] = 0
	TCCR1B = 0; // reset control register B

	/* disable clock to timer 1 */
	PRR0 |= (1 << PRTIM1);
}

/**
 * Setup timer interrupt to periodically scan the keyboard matrix
 * MUST run with interrupts disabled.
 */
static void
startKeyboardScan()
{
	/* 
	 * select operation mode for timer 1
	 * we use CTC (Clear Timer on Compare match) WGM (waveform generation mode),
	 * waveform function generator OC1{A,B,C} outputs are disconnected
	 * we use the prescaler clock source with clk/64 division
	 * we use channel A for compare and trigger an interrupt on match
	 *
	 * TCCR1A = 0;
	 * TCCR1B = 0;
	 */

	/* set output compare registers */
	OCR1A = KEYBOARD_SCAN_INTERVAL_MS * TIMER1_TICKS;
	
	TIFR1 |= (1 << OCF1A); // enable OCF1A OC1A interrupt
	TIMSK1 |= (1 << OCIE1A); // unmask OC1A interrupt

	/* WGM1[3:2] select CTC */
	TCCR1B |= (1 << WGM12) | (0 << WGM13);
	/* Select clock source and start the timer, clk/64 prescaler */
	TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);
}

static void
stopKeyboardScan()
{
	/* Mask timer interrupt */
	TIMSK1 &= ~(1 << OCIE1A);
	/* Clear clock source and stop timer */
	TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
}

int
main(void)
{
	/* disable interrupts before anything else */
	cli();

	setupHardware();
	/* 
	 * Create a regular character stream for the interface so that
	 * it can be used with the stdio.h functions
	 */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &serialStream);
	/* Shutdown leds */
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);

	initKeyboardScan();
	startKeyboardScan();

	/* enable interrupts */
	sei();

	while (true) {
		/*
		 * Must throw away unused bytes from the host,
		 * or it will lock up while waiting for the device
		 */
		CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		HID_Device_USBTask(&Keyboard_HID_Interface);
		USB_USBTask();
	}
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(
		&VirtualSerial_CDC_Interface);
	ConfigSuccess &= HID_Device_ConfigureEndpoints(
		&Keyboard_HID_Interface);

	/* Enable start-of-frame interrupts, this calls the StartOfFrame device event */
	USB_Device_EnableSOFEvents();

	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event triggered once every millisecond by the SOF interrupt */
void EVENT_USB_Device_StartOfFrame(void)
{
	/** Signals to the HID driver that a millisecond have elapsed.
	 * This is used internally to keep the idle count and handle
	 * hardware key repeats.
	 */
	HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
	HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
}

/** CDC class driver callback function that handles changes to the virtual
 *  control lines sent from the host.
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface
 *  configuration structure being referenced
 */
void EVENT_CDC_Device_ControLineStateChanged(
	USB_ClassInfo_CDC_Device_t * const CDCInterfaceInfo)
{
	/* You can get changes to the virtual CDC lines in this callback; a common
	   use-case is to use the Data Terminal Ready (DTR) flag to enable and
	   disable CDC communications in your application when set to avoid the
	   application blocking while waiting for a host to become ready and read
	   in the pending data from the USB endpoints.
	*/
	hostConnected = (
		CDCInterfaceInfo->State.ControlLineStates.HostToDevice &
		CDC_CONTROL_LINE_OUT_DTR) != 0;
	if (hostConnected) {
		fputs(banner, &serialStream);
	}
}

/** HID class driver callback function that handles keyboard report creation
 *
 * \param[in] HIDInterfaceInfo HID class driver information for the current interface
 * \param[in,out] reportID Report ID requested by the host if nonzero, otherwise the
 * callback should set it to the generated Report ID.
 * \param[in] reportType Type of the report to create.
 * \param[out] reportData Buffer where the created report should be stored.
 * \param[out] reportSize Size of the report stored to the reportData buffer, zero if
 * no report is to be sent.
 *
 * \return True to force sending of the report, false let the library decide.
 */
bool CALLBACK_HID_Device_CreateHIDReport(
	USB_ClassInfo_HID_Device_t * const HIDInterfaceInfo,
	uint8_t * const reportID,
	const uint8_t reportType,
	void *reportData,
	uint16_t * const reportSize)
{
	USB_KeyboardReport_Data_t *kbdReport = (USB_KeyboardReport_Data_t *)reportData;

	if (reportType != HID_REPORT_ITEM_In) {
		*reportSize = 0;
		DEBUG("Error: Requested report type %x is not supported\r\n",
		      reportType);
		return false;
	}

	matrixFillKeyboardReport(kbdReport);
	*reportSize = sizeof(USB_KeyboardReport_Data_t);

	return false;
}

/** HID class driver callback function that handles incoming reports from the host
 * The host may send keyboard reports to signal caps lock enable or num lock enable
 * events from other keyboards.
 *
 * \param[in] HIDInterfaceInfo HID class driver information for the current interface.
 * \param[in] reportID ID of the received report.
 * \param[in] reportType Type of the report received.
 * \param[in] reportData Buffer where the report is stored.
 * \param[in] reportSize Size of the received report.
 */
void CALLBACK_HID_Device_ProcessHIDReport(
	USB_ClassInfo_HID_Device_t * const HIDInterfaceInfo,
	const uint8_t reportID,
	const uint8_t reportType,
	const void *reportData,
	const uint16_t reportSize)
{
	if (reportType != HID_REPORT_ITEM_Out) {
		DEBUG("Error: Received report type %x is not supported\r\n",
		      reportType);
	}
}
