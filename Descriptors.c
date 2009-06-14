/*
             LUFA Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the MIT Licence
*/

/*
    Loopback demo by Opendous Inc.
    Based on LUFA demo applications by Dean Camera and Denver Gingerich.
*/

/** \file
 *
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special 
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.  
 */

#include "Descriptors.h"

 /** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
USB_Descriptor_Device_t PROGMEM DeviceDescriptor  =
{
	Header:                 {Size: sizeof(USB_Descriptor_Device_t), Type: DTYPE_Device},
		
	USBSpecification:       VERSION_BCD(01.10),
	Class:                  0x00,
	SubClass:               0x00,	
	Protocol:               0x00,
				
	Endpoint0Size:          8,
		
	VendorID:               0x03EB,
	ProductID:              0x204F,
	ReleaseNumber:          0x0001,
		
	ManufacturerStrIndex:   0x01,
	ProductStrIndex:        0x02,
	SerialNumStrIndex:      0x03,
		
	NumberOfConfigurations: 1
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor  =
{
	Config:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Configuration_Header_t), Type: DTYPE_Configuration},

			TotalConfigurationSize: sizeof(USB_Descriptor_Configuration_t),
			TotalInterfaces:        1,
				
			ConfigurationNumber:    1,
			ConfigurationStrIndex:  NO_DESCRIPTOR,
				
			ConfigAttributes:       (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELFPOWERED),
			
			MaxPowerConsumption:    USB_CONFIG_POWER_MA(100)
		},
		
	Interface:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:        0x00,
			AlternateSetting:       0x00,
			
			TotalEndpoints:         2,
				
			Class:                  0xFF,
			SubClass:               0x00,
			Protocol:               0x00,
				
			InterfaceStrIndex:      NO_DESCRIPTOR
		},

	DataINEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},

			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_IN | IN_EP),
			Attributes:             EP_TYPE_BULK,
			EndpointSize:           IN_EP_SIZE,
			PollingIntervalMS:      0x00
		},

	DataOUTEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},

			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_OUT | OUT_EP),
			Attributes:             EP_TYPE_BULK,
			EndpointSize:           OUT_EP_SIZE,
			PollingIntervalMS:      0x00
		}
};

/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
USB_Descriptor_String_t PROGMEM LanguageString  =
{
	Header:                 {Size: USB_STRING_LEN(1), Type: DTYPE_String},
		
	UnicodeString:          {LANGUAGE_ID_ENG}
};

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
USB_Descriptor_String_t PROGMEM ManufacturerString  =
{
	Header:                 {Size: USB_STRING_LEN(19), Type: DTYPE_String},
		
	UnicodeString:          L"www.AVRopendous.org"
};

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
USB_Descriptor_String_t PROGMEM ProductString  =
{
	Header:                 {Size: USB_STRING_LEN(8), Type: DTYPE_String},

	UnicodeString:          L"LUFA JTAG"
};

/** Product serial number string. This is a Unicode string containing the product's serial number
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
USB_Descriptor_String_t PROGMEM SerialNumberString  =
{
	Header:                 {Size: USB_STRING_LEN(12), Type: DTYPE_String},
		
	UnicodeString:          L"000000000000"
};

/** This function is called by the library when in device mode, and must be overridden (see StdDescriptors.h
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device recieves a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	void*    Address = NULL;
	uint16_t Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = (void*)&DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = (void*)&ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (DescriptorNumber)
			{
				case 0x00:
					Address = (void*)&LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01:
					Address = (void*)&ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case 0x02:
					Address = (void*)&ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
				case 0x03:
					Address = (void*)&SerialNumberString;
					Size    = pgm_read_byte(&SerialNumberString.Header.Size);
					break;
			}

			break;
	}

	*DescriptorAddress = Address;	
	return Size;
}
