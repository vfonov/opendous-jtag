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
#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

	/* Includes: */
		#include <LUFA/Drivers/USB/USB.h>
		#include <avr/pgmspace.h>

	/* Macros: */
		#define IN_EP                       1
		#define OUT_EP                      2
		#define IN_EP_SIZE                  64
		#define OUT_EP_SIZE                 64

	/* Type Defines: */
		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;
			USB_Descriptor_Interface_t            Interface;
			USB_Descriptor_Endpoint_t             DataINEndpoint;
			USB_Descriptor_Endpoint_t             DataOUTEndpoint;
		} USB_Descriptor_Configuration_t;

	/* External Variables: */
		extern USB_Descriptor_Configuration_t ConfigurationDescriptor;

	/* Function Prototypes: */
		uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress)
		                           ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif
