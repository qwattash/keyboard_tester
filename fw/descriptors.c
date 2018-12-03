/*
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
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.
 */

#include <avr/pgmspace.h>
#include "descriptors.h"

/* Manufacturer string descriptor, unicode string */
const USB_Descriptor_String_t PROGMEM ManufacturerString =
	USB_STRING_DESCRIPTOR(L"qwattash");
/* Product string descriptor, unicode string */
const USB_Descriptor_String_t PROGMEM ProductString =
	USB_STRING_DESCRIPTOR(L"PinKey Keyboard prototype board");
/* Language string descriptor, unicode string, actually a list of shorts */
const USB_Descriptor_String_t PROGMEM LanguageString =
	USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
/* String descriptor for the default configuration descriptor, unicode string */
const USB_Descriptor_String_t PROGMEM DefaultConfigNameString =
	USB_STRING_DESCRIPTOR(L"Default");

/**
 * USB device descriptor.
 * This is the first thing fetched from the USB bus driver to
 * get the device capabilities and identity.
 */
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},
	.USBSpecification = VERSION_BCD(1,1,0),
	.Class = CDC_CSCP_CDCClass,
	.SubClass = USB_CSCP_NoDeviceSubclass,
	.Protocol = USB_CSCP_NoDeviceProtocol,
	.Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE, /* Platform specific size (see LUFAConfig) */
	.VendorID = 0x03EB, // Atmel corp.
	.ProductID = 0x2042, // LUFA Keyboard demo
	.ReleaseNumber = VERSION_BCD(1,0,0),
	.ManufacturerStrIndex = STRING_ID_Manufacturer,
	.ProductStrIndex = STRING_ID_Product,
	.SerialNumStrIndex = USE_INTERNAL_SERIAL,
	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_HIDReport_Datatype_t PROGMEM KeyboardReport[] =
{
	// USB class driver standard Keyboard report, with 6 max simultaneous key presses
	HID_DESCRIPTOR_KEYBOARD(6)
};

/**
 * Custom type for this device.
 * Represent the device configuration(s) available to the bus/keyboard USB driver.
 */
const USB_Descriptor_Configuration_t PROGMEM ConfigDescriptor =
{
	/* Standard-defined header */
	.Config = {
		.Header = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration },
		.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
		.TotalInterfaces = 3, // number of interfaces in this configuration
		.ConfigurationNumber = 1, // config index of this configuration
		.ConfigurationStrIndex = STRING_ID_ConfigDefault, // index of string descriptor describing this configuration
		.ConfigAttributes = USB_CONFIG_ATTR_RESERVED,
		.MaxPowerConsumption = USB_CONFIG_POWER_MA(100)
	},
	// CDC serial interface configuration
	.CDC_CCI_Interface = {
		.Header = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},
		.InterfaceNumber = INTERFACE_ID_CDC_CCI,
		.AlternateSetting = 0,
		.TotalEndpoints = 1,
		.Class = CDC_CSCP_CDCClass,
		.SubClass = CDC_CSCP_ACMSubclass,
		.Protocol = CDC_CSCP_ATCommandProtocol,
		.InterfaceStrIndex = NO_DESCRIPTOR
	},
	.CDC_Functional_Header = {
		.Header = {.Size = sizeof(USB_CDC_Descriptor_FunctionalHeader_t), .Type = CDC_DTYPE_CSInterface},
		.Subtype = CDC_DSUBTYPE_CSInterface_Header,
		.CDCSpecification = VERSION_BCD(1,1,0),
	},
	.CDC_Functional_ACM = {
		.Header = {.Size = sizeof(USB_CDC_Descriptor_FunctionalACM_t), .Type = CDC_DTYPE_CSInterface},
		.Subtype = CDC_DSUBTYPE_CSInterface_ACM,
		.Capabilities = 0x06, // ??
	},
	.CDC_Functional_Union = {
		.Header = {.Size = sizeof(USB_CDC_Descriptor_FunctionalUnion_t), .Type = CDC_DTYPE_CSInterface},
		.Subtype = CDC_DSUBTYPE_CSInterface_Union,
		.MasterInterfaceNumber = INTERFACE_ID_CDC_CCI,
		.SlaveInterfaceNumber = INTERFACE_ID_CDC_DCI,
	},
	.CDC_NotificationEndpoint = {
		.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
		.EndpointAddress = CDC_NOTIFICATION_EPADDR,
		.Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize = CDC_NOTIFICATION_EPSIZE,
		.PollingIntervalMS = 0xFF
	},
	.CDC_DCI_Interface = {
		.Header = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},
		.InterfaceNumber = INTERFACE_ID_CDC_DCI,
		.AlternateSetting = 0,
		.TotalEndpoints = 2,
		.Class = CDC_CSCP_CDCDataClass,
		.SubClass = CDC_CSCP_NoDataSubclass,
		.Protocol = CDC_CSCP_NoDataProtocol,
		.InterfaceStrIndex = NO_DESCRIPTOR
	},
	.CDC_DataOutEndpoint = {
		.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
		.EndpointAddress = CDC_RX_EPADDR,
		.Attributes = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize = CDC_TXRX_EPSIZE,
		.PollingIntervalMS = 0x05
	},
	.CDC_DataInEndpoint = {
		.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
		.EndpointAddress = CDC_TX_EPADDR,
		.Attributes = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize = CDC_TXRX_EPSIZE,
		.PollingIntervalMS = 0x05
	},
	// Keyboard interface configuration
	.HID_Interface = {
		.Header = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},
		.InterfaceNumber = INTERFACE_ID_Keyboard, // interface index in current config
		// alternate setting for the interface number, same number can have
		// multiple settings with different endpoint configs
		.AlternateSetting = 0x00,
		.TotalEndpoints = 1,
		.Class = HID_CSCP_HIDClass, // device interface belongs to the HID class
		.SubClass = HID_CSCP_BootSubclass, // device interface implements the HID boot protocol
		.Protocol = HID_CSCP_KeyboardBootProtocol, // device interface belongs to the HID keyboard boot protocol
		.InterfaceStrIndex = NO_DESCRIPTOR
	},
	.HID_Keyboard = {
		.Header = {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID},
		.HIDSpec = VERSION_BCD(1, 1, 1),
		.CountryCode = 0x00, // 0x00 if universal
		.TotalReportDescriptors = 1, // num report endpoint descriptors for the interface
		.HIDReportType = HID_DTYPE_Report,
		.HIDReportLength = sizeof(KeyboardReport), // Length of associated report descriptor		
	},
	.HID_ReportEndpoint = {
		.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
		.EndpointAddress = HID_REPORT_IN_EPADDR,
		.Attributes = (
			EP_TYPE_INTERRUPT |
			ENDPOINT_ATTR_NO_SYNC | // endpoint not synchronized
			ENDPOINT_USAGE_DATA), // used for data transfers
		.EndpointSize = HID_REPORT_EPSIZE, // Endpoint bank size, maximum size of data packet that can be received
		.PollingIntervalMS = 0x05 // polling interval for INTERRUPT/ISOSYNCHRONOUS endpoints
	}
};

/**
 * Callback invoked by LUFA to fetch descriptors from the control endpoint.
 */
uint16_t CALLBACK_USB_GetDescriptor(
	const uint16_t value,
	const uint16_t index,
	const void** const descriptorAddress,
	uint8_t* const descriptorMemorySpace)
{
  // descriptor type and index
  const uint8_t dtype = (value >> 8) & 0xFF;
  const uint8_t dindex = (value & 0xFF);

  const void* address = NULL;
  uint16_t dsize = NO_DESCRIPTOR;

  switch (dtype) {
  case DTYPE_Device:
    address = &DeviceDescriptor;
    dsize = sizeof(DeviceDescriptor);
    break;
  case DTYPE_Configuration:
    address = &ConfigDescriptor;
    dsize = sizeof(ConfigDescriptor);
    break;
  case DTYPE_String:
    switch(dindex) {
    case STRING_ID_Language:
      address = &LanguageString;
      dsize = sizeof(LanguageString);
      break;
    case STRING_ID_Product:
      address = &ProductString;
      dsize = sizeof(ProductString);
      break;
    case STRING_ID_Manufacturer:
      address = &ManufacturerString;
      dsize = sizeof(ManufacturerString);
      break;
    case STRING_ID_ConfigDefault:
      address = &DefaultConfigNameString;
      dsize = sizeof(DefaultConfigNameString);
      break;
    }
    break;
  case HID_DTYPE_HID:
    address = &ConfigDescriptor.HID_Keyboard;
    dsize = sizeof(ConfigDescriptor.HID_Keyboard);
    break;
  case HID_DTYPE_Report:
    address = &KeyboardReport;
    dsize = sizeof(KeyboardReport);
    break;
  }

  *descriptorMemorySpace = MEMSPACE_FLASH;
  *descriptorAddress = address;
  return dsize;
}

