/*
  Copyright 2018  Dean Camera (dean [at] fourwalledcubicle [dot] com)
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

/**
 *  Keyboard tester usb descriptors. Lifted from LUFA demo.
 */

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

#include <LUFA/Drivers/USB/USB.h>

/** Endpoint address of the CDC device-to-host notification IN endpoint. */
#define CDC_NOTIFICATION_EPADDR        (ENDPOINT_DIR_IN | 2)

/** Endpoint address of the CDC device-to-host data IN endpoint. */
#define CDC_TX_EPADDR                  (ENDPOINT_DIR_IN | 3)

/** Endpoint address of the CDC host-to-device data OUT endpoint. */
#define CDC_RX_EPADDR                  (ENDPOINT_DIR_OUT | 4)

/** Endpoint address of the HID Report input endpoint */
#define HID_REPORT_IN_EPADDR           (ENDPOINT_DIR_IN | 5)

/** Size in bytes of the HID Report IN endpoint */
#define HID_REPORT_EPSIZE              8

/** Size in bytes of the CDC device-to-host notification IN endpoint. */
#define CDC_NOTIFICATION_EPSIZE        8

/** Size in bytes of the CDC data IN and OUT endpoints. */
#define CDC_TXRX_EPSIZE                16

/** Device configuration descriptor structure.
 * This specifies the device configuration descriptor structure that is returned
 * to the host to inform it about the configuration of the device.
 * This type is user-defined because the configuration varies for each device.
 */
typedef struct
{
	USB_Descriptor_Configuration_Header_t Config;

	// Serial interface
	// CDC Control Interface
	USB_Descriptor_Interface_t CDC_CCI_Interface;
	USB_CDC_Descriptor_FunctionalHeader_t CDC_Functional_Header;
	USB_CDC_Descriptor_FunctionalACM_t CDC_Functional_ACM;
	USB_CDC_Descriptor_FunctionalUnion_t CDC_Functional_Union;
	USB_Descriptor_Endpoint_t CDC_NotificationEndpoint;

	// CDC Data Interface
	USB_Descriptor_Interface_t CDC_DCI_Interface;
	USB_Descriptor_Endpoint_t CDC_DataOutEndpoint;
	USB_Descriptor_Endpoint_t CDC_DataInEndpoint;
	
	// Keyboard HID interface
	USB_Descriptor_Interface_t HID_Interface;
	USB_HID_Descriptor_HID_t HID_Keyboard;
	USB_Descriptor_Endpoint_t HID_ReportEndpoint;
  
} USB_Descriptor_Configuration_t;

/**
 * ID of string descriptors for referencing in parent descriptors.
 */
enum StringDescriptor_t
{
	STRING_ID_Language = 0,
	STRING_ID_Product = 1,
	STRING_ID_Manufacturer = 2,
	STRING_ID_ConfigDefault = 3,
};

/**
 * Interface IDs in default configuration
 */
enum DefaultInterfaceNumber_t
{
	INTERFACE_ID_Keyboard = 0, /**< Keyboard interface descriptor ID */
	INTERFACE_ID_CDC_CCI = 1, /**< CDC CCI interface descriptor ID */
	INTERFACE_ID_CDC_DCI = 2, /**< CDC DCI interface descriptor ID */
};

#endif

