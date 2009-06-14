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
	#include <LUFA/Drivers/USB/USB.h>            // USB Functionality
	#include <LUFA/Scheduler/Scheduler.h>		// Simple scheduler for task management

	/* Macros: */

	/* Type Defines: */

	/* Global Variables: */

	/* Task Definitions: */
	TASK(USB_MainTask);

	/* Event Handlers: */
	void EVENT_USB_Connect(void);
	void EVENT_USB_Reset(void);
	void EVENT_USB_Disconnect(void);
	void EVENT_USB_ConfigurationChanged(void);
	void EVENT_USB_UnhandledControlPacket(void);

	/* Function Prototypes: */

#endif //JTAG
