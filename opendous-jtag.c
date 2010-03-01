/*
		opendous-jtag by Vladimir S. Fonov, based on
    eStick-jtag, by Cahya Wirawan <cahya@gmx.at> 
    Based on opendous-jtag by Vladimir Fonov and LUFA demo applications by Dean Camera and Denver Gingerich.
    Released under the MIT Licence.
*/

#include "opendous-jtag.h"
#include "jtag_defs.h"
#include "jtag_functions.h"

#ifdef DEBUG
#include <LUFA/Drivers/Peripheral/SerialStream.h>
#include <stdio.h>
#endif //DEBUG

/* Global Variables */

uint8_t  usbBuffer[OPENDOUS_USB_BUFFER_SIZE+OPENDOUS_USB_BUFFER_OFFSET];
uint8_t  *dataFromHost = usbBuffer+OPENDOUS_USB_BUFFER_OFFSET;
uint8_t  *dataToHost = usbBuffer;
uint16_t dataFromHostSize=0;
uint16_t dataToHostSize=0;

volatile uint8_t resetJtagTransfers=0;

/** Main program entry point. This routine configures the hardware required by the application, then
 *  starts the scheduler to run the USB management task.
 */
int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	clock_prescale_set(clock_div_1);

  jtag_init();
  
  //DEBUG
#ifdef DEBUG
  SerialStream_Init(9600,0);
#endif //DEBUG
  
	// initialize the send and receive buffers
	uint16_t i = 0;
	for (i = 0; i < OPENDOUS_OUT_BUFFER_SIZE; i++) {
		dataFromHost[i] = 0;
	}

	for (i = 0; i < OPENDOUS_IN_BUFFER_SIZE; i++) {
		dataToHost[i] = 0;
	}
  dataFromHostSize=0;
  dataToHostSize=0;
  resetJtagTransfers=0;

	/* Initialize USB Subsystem */
	USB_Init();

#ifdef DEBUG
	printf("Starting OPENDOUS-jtag\r\n");
#endif //DEBUG
	while(1)
	{
		JTAG_Task();
		USB_USBTask();
	}
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Device_Connect(void)
{
#ifdef DEBUG
	printf("Starting OPENDOUS-connected\r\n");
#endif //DEBUG
}


/** Event handler for the USB_Reset event. This fires when the USB interface is reset by the USB host, before the
 *  enumeration process begins, and enables the control endpoint interrupt so that control requests can be handled
 *  asynchronously when they arrive rather than when the control endpoint is polled manually.
 */
void EVENT_USB_Device_Reset(void)
{
	//TODO: reset JTAG control(?)
#ifdef DEBUG
	printf("Starting OPENDOUS-reset\r\n");
#endif //DEBUG
}


/** Event handler for the USB_Disconnect event.
 */
void EVENT_USB_Device_Disconnect(void)
{
#ifdef DEBUG
	printf("Starting OPENDOUS-disconnected\r\n");
#endif //DEBUG

}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the keyboard device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	/* Setup Keyboard Keycode Report Endpoint */
	Endpoint_ConfigureEndpoint(IN_EP, EP_TYPE_BULK,
								ENDPOINT_DIR_IN, IN_EP_SIZE,
								ENDPOINT_BANK_SINGLE);

	/* Setup Keyboard LED Report Endpoint */
	Endpoint_ConfigureEndpoint(OUT_EP, EP_TYPE_BULK,
								ENDPOINT_DIR_OUT, OUT_EP_SIZE,
								ENDPOINT_BANK_SINGLE);

  
  // pull lines TRST and SRST high
  jtag_init();
#ifdef DEBUG
	printf("Starting OPENDOUS-conf changed\r\n");
#endif //DEBUG
  
}


//TODO: add possibility to abor current JTAG sequence and reset the pins
void EVENT_USB_Device_UnhandledControlRequest(void)
{
	switch (USB_ControlRequest.bRequest)
	{
	}
#ifdef DEBUG
	printf("Starting OPENDOUS-EVENT_USB_Device_UnhandledControlRequest\r\n");
#endif //DEBUG

}

void JTAG_Task(void)
{	
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;

	/* Check if the USB System is connected to a Host */
	//if (USB_DeviceState == DEVICE_STATE_Configured  )
	{
    Endpoint_SelectEndpoint(IN_EP);

    if(dataToHostSize && Endpoint_IsReadWriteAllowed())
    {
#ifdef DEBUG
			printf("Sending to host :%d \r\n",dataToHostSize);
#endif //DEBUG
      Endpoint_Write_Stream_LE(dataToHost,dataToHostSize);
    
      /* Handshake the IN Endpoint - send the data to the host */
      Endpoint_ClearIN();
      
      dataToHostSize=0;
    }

    Endpoint_SelectEndpoint(OUT_EP);

    if(Endpoint_IsReadWriteAllowed())
    {
		  dataFromHostSize = Endpoint_Read_Word_LE();
#ifdef DEBUG
			printf("Data :%d ",dataFromHostSize);
#endif //DEBUG
		  Endpoint_Read_Stream_LE(dataFromHost, dataFromHostSize);
		  Endpoint_ClearOUT();

		  if(dataFromHostSize>0)
      {        
        //first byte is always the command
        dataFromHostSize--;
        
        dataToHostSize=0;
        
        switch( dataFromHost[0] &JTAG_CMD_MASK ) 
        {
          
        case JTAG_CMD_TAP_OUTPUT:
          
#ifdef DEBUG
					printf("JTAG_CMD_TAP_OUTPUT\r\n ");
#endif //DEBUG
          dataFromHostSize*=4;

          if( dataFromHost[0] & JTAG_DATA_MASK )
            dataFromHostSize-= (4- ((dataFromHost[0] & JTAG_DATA_MASK)>>4));
          if(jtag_delay)
            dataToHostSize= jtag_tap_output_with_delay( &dataFromHost[1] , dataFromHostSize, dataToHost);
          else
            dataToHostSize= jtag_tap_output_max_speed( &dataFromHost[1] , dataFromHostSize, dataToHost);
          break;
          
        case JTAG_CMD_TAP_OUTPUT_EMU:
#ifdef DEBUG
					printf("JTAG_CMD_TAP_OUTPUT_EMU\r\n ");
#endif //DEBUG
          dataFromHostSize*=4;
          if(dataFromHost[0]&JTAG_DATA_MASK)
            dataFromHostSize-=(4- ((dataFromHost[0]&JTAG_DATA_MASK)>>4));
          
          dataToHostSize=jtag_tap_output_emu(&dataFromHost[1], dataFromHostSize, dataToHost);
          
          break;
          
        case JTAG_CMD_READ_INPUT:
#ifdef DEBUG
					printf("JTAG_CMD_READ_INPUT\r\n ");
#endif //DEBUG
          dataToHost[0]=jtag_read_input();
          dataToHostSize=1;
          break;
        
        case JTAG_CMD_SET_SRST:
#ifdef DEBUG
					printf("JTAG_CMD_SET_SRST\r\n ");
#endif //DEBUG
          jtag_set_srst(dataFromHost[1]&1);
          dataToHost[0]=0;//TODO: what to output here?
          dataToHostSize=1;
          break;
        
        case JTAG_CMD_SET_TRST:
#ifdef DEBUG
					printf("JTAG_CMD_SET_TRST\r\n ");
#endif //DEBUG
          jtag_set_trst(dataFromHost[1]&1);
          dataToHost[0]=0;//TODO: what to output here?
          dataToHostSize=1;
          break;
        
        case JTAG_CMD_SET_DELAY:
#ifdef DEBUG
					printf("JTAG_CMD_SET_DELAY\r\n ");
#endif //DEBUG
          jtag_delay=dataFromHost[1]*256;
          dataToHost[0]=0;//TODO: what to output here?
          dataToHostSize=1;
          break;
        case JTAG_CMD_SET_SRST_TRST:
#ifdef DEBUG
					printf("JTAG_CMD_SET_SRST_TRST\r\n ");
#endif //DEBUG
          jtag_set_trst_srst(dataFromHost[1]&2?1:0,dataFromHost[1]&1);
          dataToHost[0]=0;//TODO: what to output here?
          dataToHostSize=1;
        	break;
				case JTAG_CMD_READ_CONFIG:
#ifdef DEBUG
					printf("JTAG_CMD_READ_CONFIG\r\n ");
#endif //DEBUG
					dataToHostSize=jtag_read_config(&dataToHost[0]);
					break;
					
        default: //REPORT ERROR?
#ifdef DEBUG
					printf("Unknown command\r\n ");
#endif //DEBUG
          break;
        }
      }
    }
	}
}
