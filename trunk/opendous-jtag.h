/*
		opendous_jtag version 0.2 by Vladimir S. Fonov with improvements from
    eStick-jtag, by Cahya Wirawan <cahya@gmx.at> 
    Based on opendous-jtag by Vladimir Fonov and LUFA demo applications by Dean Camera and Denver Gingerich.
    Released under the MIT Licence.
*/

#ifndef _OPENDOUS_JTAG_H_
#define _OPENDOUS_JTAG_H_

	/* Includes: */
	#include <avr/io.h>
	#include <avr/wdt.h>
	#include <avr/power.h>
	#include <util/delay_basic.h>
	#include "Descriptors.h"

	#include <LUFA/Version.h>				// Library Version Information
	#include <LUFA/Drivers/USB/USB.h>            // USB Functionality
	#include <LUFA/Scheduler/Scheduler.h>		// Simple scheduler for task management

	/* Macros: */

	/* Type Defines: */
	
	#define OPENDOUS_USB_BUFFER_OFFSET 2
	#define OPENDOUS_IN_BUFFER_SIZE	  (OPENDOUS_USB_BUFFER_SIZE)
	#define OPENDOUS_OUT_BUFFER_SIZE  (OPENDOUS_USB_BUFFER_SIZE)

	/* Global Variables: */

	/* Task Definitions: */
	void JTAG_Task(void);

	/* Event Handlers: */
	void EVENT_USB_Device_Connect(void);
	void EVENT_USB_Device_Reset(void);
	void EVENT_USB_Device_Disconnect(void);
	void EVENT_USB_Device_ConfigurationChanged(void);
	void EVENT_USB_Device_UnhandledControlPacket(void);

	/* Function Prototypes: */

#endif //OPENDOUS_JTAG
