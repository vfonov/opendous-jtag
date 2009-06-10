/*
     Copyright (C) Vladimir Fonov, 2009.
              
 vladimir <dot> fonov <at> gmail <dot> com
      www.ilmarin.info

 Released under the MIT Licence
*/

/*
    JTAG 
    Based on LUFA demo applications by Dean Camera and Denver Gingerich.
*/

#ifndef _JTAG_H_
#define _JTAG_H_

	/* Includes: */
	#include <avr/io.h>
	#include <avr/wdt.h>
	#include <avr/power.h>
	#include <util/delay_basic.h>
	#include "Descriptors.h"

	#include <LUFA/Version.h>				// Library Version Information
	#include <LUFA/Common/ButtLoadTag.h>		// PROGMEM tags readable by the ButtLoad project
	#include <LUFA/Drivers/USB/USB.h>            // USB Functionality
	#include <LUFA/Scheduler/Scheduler.h>		// Simple scheduler for task management

	/* Macros: */

	/* Type Defines: */

	/* Global Variables: */

	/* Task Definitions: */
	TASK(USB_MainTask);

	/* Event Handlers: */
	HANDLES_EVENT(USB_Connect);
	HANDLES_EVENT(USB_Reset);
	HANDLES_EVENT(USB_Disconnect);
	HANDLES_EVENT(USB_ConfigurationChanged);
	HANDLES_EVENT(USB_UnhandledControlPacket);

	/* Function Prototypes: */

#endif //JTAG
