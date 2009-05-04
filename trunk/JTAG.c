/*
     Copyright (C) Vladimir Fonov, 2009.
              
 vladimir <dot> fonov <at> gmail <dot> com
      www.ilmarin.info

 Released under the MIT Licence
*/

/*
    JTAG2 
    Based on LUFA demo applications by Dean Camera and Denver Gingerich.
*/

#include "JTAG2.h"

#ifdef DEBUG
#include <LUFA/Drivers/AT90USBXXX/Serial_Stream.h>
#include <stdio.h>
#endif //DEBUG

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "JTAG2 App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(LUFAVersion, "LUFA V" LUFA_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask         , TaskStatus: TASK_STOP },
	{ Task: USB_MainTask        , TaskStatus: TASK_STOP },
};

/* Global Variables */
uint8_t  dataFromHost[IN_EP_SIZE];
uint8_t  dataToHost[OUT_EP_SIZE];
uint16_t dataFromHostSize=0;
uint16_t dataToHostSize=0;
uint8_t  jtag_delay=0;

volatile uint8_t resetJtagTransfers=0;

//jtag i/o pins
#define JTAG_OUT PORTB
#define JTAG_IN  PINB
#define JTAG_DIR DDRB

//ouput pins
#define JTAG_PIN_TDI  0
#define JTAG_PIN_TMS  1
#define JTAG_PIN_TRST 2
#define JTAG_PIN_SRST 3
#define JTAG_PIN_CLK  4
//input pins
#define JTAG_PIN_TDO  5
#define JTAG_PIN_EMU  6

//JTAG usb commands
//TODO: ADD commands to deal with RTCK ?
//TODO: maybe add commands to query some firmware info...
#define JTAG_CMD_TAP_OUTPUT     0x0
#define JTAG_CMD_SET_TRST       0x1
#define JTAG_CMD_SET_SRST       0x2
#define JTAG_CMD_READ_INPUT     0x3
#define JTAG_CMD_TAP_OUTPUT_EMU 0x4
#define JTAG_CMD_SET_DELAY      0x5

//JTAG usb command mask
#define JTAG_CMD_MASK       0x0f
#define JTAG_DATA_MASK      0xf0

//JTAG pins masks
#define JTAG_OUTPUT_MASK ((1<<JTAG_PIN_TDI)|(1<<JTAG_PIN_TMS)|(1<<JTAG_PIN_TRST)|(1<<JTAG_PIN_SRST)|(1<<JTAG_PIN_CLK))
#define JTAG_INPUT_MASK  ((1<<JTAG_PIN_TDO)|(1<<JTAG_PIN_EMU))
#define JTAG_SIGNAL_MASK ((1<<JTAG_PIN_TDI)|(1<<JTAG_PIN_TMS))

#define JTAG_CLK_LO  ~(1<<JTAG_PIN_CLK)
#define JTAG_CLK_HI   (1<<JTAG_PIN_CLK)

//additional delay to make clk hi and lo approximately the same length, not sure if this is really needed
#define JTAG_DELAY2 20

//! initialize JTAG interface
void jtag_init(void)
{
  JTAG_OUT=JTAG_PIN_TRST|JTAG_PIN_SRST; //passive state high
  JTAG_DIR=JTAG_OUTPUT_MASK; 
  dataFromHostSize=0;
  dataToHostSize=0;
  resetJtagTransfers=0;
}

//! send taps through JTAG interface and recieve responce from TDO pin only
//! \parameter out_buffer - buffer of taps for output, data is packed TDI and TMS values a stored together 
//! \parameter out_length - total number of pairs to send (maximum length is 4*255 samples)
//! \parameter in_buffer  - buffer which will hold recieved data data will be packed 
//! \return    number of bytes used in the in_buffer 
uint8_t jtag_tap_output(const uint8_t *out_buffer, uint16_t out_length, uint8_t *in_buffer)
{
  uint16_t i;
  
#ifdef      DEBUG
  printf("Sending %d bits \r\n", dataFromHostSize);
#endif
  
  for(i=0 ; i<out_length ; i++ )
  {
    uint8_t index=i/4;
    uint8_t bit=  (i%4)*2;

    uint8_t index2=i/8;
    uint8_t bit2=  i%8;

    uint8_t tdi=(out_buffer[index]>>bit    )&1;
    uint8_t tms=(out_buffer[index]>>(bit+1))&1;

    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) )
               | (tdi<<JTAG_PIN_TDI)
               | (tms<<JTAG_PIN_TMS);

    if(jtag_delay>0) _delay_loop_1(jtag_delay);

    JTAG_OUT|=JTAG_CLK_HI;//CLK hi

    _delay_loop_1(jtag_delay+JTAG_DELAY2);

    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    uint8_t data=JTAG_IN;
    
    if(!bit2)
      in_buffer[index2]=0;
    
    in_buffer[index2] |= ((data>>JTAG_PIN_TDO)&1)<<bit2;
  }
  
  return (out_length+7)/8;
}

//! send taps through JTAG interface and recieve responce from TDO and EMU pins 
//! \parameter out_buffer - buffer of taps for output, data is packed TDI and TMS values a stored together 
//! \parameter out_length - total number of pairs to send (maximum length is 4*255 samples)
//! \parameter in_buffer  - buffer which will hold recieved data data will be packed 
//! \return    number of bytes used in the in_buffer (equal to the input (length+3)/4
uint8_t jtag_tap_output_emu(const uint8_t *out_buffer,uint16_t out_length,uint8_t *in_buffer)
{
  uint16_t i;
  
  for(i=0 ; i<out_length ; i++ )
  {
    uint8_t index=i/4;
    uint8_t bit=(i%4)*2;

    uint8_t tdi=(out_buffer[index]>>bit)&1;
    uint8_t tms=(out_buffer[index]>>(bit+1))&1;

    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) )
        | (tdi<<JTAG_PIN_TDI)
        | (tms<<JTAG_PIN_TMS);

    if(jtag_delay>0) _delay_loop_1(jtag_delay);

    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    _delay_loop_1(jtag_delay+JTAG_DELAY2);


    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    uint8_t data=JTAG_IN;
    
    if(!bit)
      in_buffer[index]=0;
    
    in_buffer[index] |= ((data>>JTAG_PIN_TDO)&1)<<(bit) |
       ((data>>JTAG_PIN_EMU)&1)<<(bit+1);
  }
  return (out_length+3)/4;
}

//! return current status of TDO & EMU pins
//! \return packed result TDO - bit 0 , EMU bit 1
uint8_t jtag_read_input(void)
{
    uint8_t data=JTAG_IN;
    return ((data>>JTAG_PIN_TDO)&1)|(((data>>JTAG_PIN_EMU)&1)<<1);
} 

//! set pin TRST 
void jtag_set_trst(uint8_t trst)
{
  JTAG_OUT= (JTAG_OUT&(~(1<<JTAG_PIN_TRST)))|(trst<<JTAG_PIN_TRST);
} 

//! set pin SRST 
void jtag_set_srst(uint8_t srst)
{
  JTAG_OUT=(JTAG_OUT&(~(1<<JTAG_PIN_SRST))) |(srst<<JTAG_PIN_SRST);
} 


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

	/* Hardware Initialization */
	DDRD = 0;
	PORTD = 0;
	DDRB = 0;
	PORTB = 0;
	DDRC |= ((0 << PC2) | (0 << PC4) | (0 << PC5) | (0 << PC6) | (0 << PC7));  //AT90USBxx2
	PORTC |= ((0 << PC2) | (0 << PC4) | (0 << PC5) | (0 << PC6) | (0 << PC7)); //AT90USBxx2
	#if (BOARD == BOARD_USBKEY)
		DDRA = 0;
		PORTA = 0;
		DDRE = 0;
		PORTE = 0;
		DDRF = 0;
		PORTF = 0;
		DDRC = 0;
		PORTC = 0;
	#endif
  
  //HWB
  DDRD = 1;
  PORTD = (1 << PB7); // only PB7(HWB) should be High as this is the bootloader pin

  jtag_init();

  //DEBUG
#ifdef DEBUG
  SerialStream_Init(9600,0);
#endif //DEBUG
  
	// initialize the send and receive buffers
	uint8_t i = 0;
	for (i = 0; i < IN_EP_SIZE; i++) {
		dataFromHost[i] = 0;
	}

	for (i = 0; i < OUT_EP_SIZE; i++) {
		dataToHost[i] = 0;
	}

 	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialize USB Subsystem */
	USB_Init();
#ifdef DEBUG
	printf("Starting JTAG\r\n");
#endif //DEBUG
	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
EVENT_HANDLER(USB_Connect)
{
 	/* Start USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);
	Scheduler_SetTaskMode(USB_MainTask, TASK_RUN);
}


/** Event handler for the USB_Reset event. This fires when the USB interface is reset by the USB host, before the
 *  enumeration process begins, and enables the control endpoint interrupt so that control requests can be handled
 *  asynchronously when they arrive rather than when the control endpoint is polled manually.
 */
EVENT_HANDLER(USB_Reset)
{
	/* Select the control endpoint */
	Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);

	/* Enable the endpoint SETUP interrupt ISR for the control endpoint */
	USB_INT_Enable(ENDPOINT_INT_SETUP);
}


/** Event handler for the USB_Disconnect event.
 */
EVENT_HANDLER(USB_Disconnect)
{
	/* Stop running keyboard reporting and USB management tasks */
	Scheduler_SetTaskMode(USB_MainTask, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the keyboard device endpoints.
 */
EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup Keyboard Keycode Report Endpoint */
	Endpoint_ConfigureEndpoint(IN_EP, EP_TYPE_BULK,
								ENDPOINT_DIR_IN, IN_EP_SIZE,
								ENDPOINT_BANK_SINGLE);

	/* Enable the endpoint IN interrupt ISR for data being sent TO the host */
	//USB_INT_Enable(ENDPOINT_INT_IN);

	/* Setup Keyboard LED Report Endpoint */
	Endpoint_ConfigureEndpoint(OUT_EP, EP_TYPE_BULK,
								ENDPOINT_DIR_OUT, OUT_EP_SIZE,
								ENDPOINT_BANK_SINGLE);

	/* Enable the endpoint OUT interrupt ISR for data recevied FROM the host */
	//USB_INT_Enable(ENDPOINT_INT_OUT);

	/* start USB_USBTask */
	Scheduler_SetTaskMode(USB_MainTask, TASK_RUN);
  
  // pull lines TRST and SRST high
  JTAG_OUT=(1<<JTAG_PIN_TRST)|(1<<JTAG_PIN_SRST);
  
}


//TODO: add possibility to abor current JTAG sequence and reset the pins

/** Event handler for the USB_UnhandledControlPacket event. This is used to catch standard and class specific
 *  control requests that are not handled internally by the USB library (including the HID commands, which are
 *  all issued via the control endpoint), so that they can be handled appropriately for the application.
 */
HANDLES_EVENT(USB_UnhandledControlPacket)
{
	//NOTE - this is here as a template only, LoopBack does not make use of it

	/* Handle HID Class specific requests here (these are Control EP requests) */
	switch (bRequest)
	{
		case 0x01:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();

        //TODO add reset code here
				/* Write the report data to the control endpoint */
				//Endpoint_Write_Control_Stream_LE(&dataToSend, sizeof(dataToSend));
        
				/* Finalize the transfer, acknowedge the host error or success OUT transfer */
				Endpoint_ClearSetupOUT();
			}

			break;
	}
}

ISR(ENDPOINT_PIPE_vect)
{
}

TASK(USB_MainTask)
{
	/* Check if the USB System is connected to a Host */
	if (USB_IsConnected)
	{
		/* process data or do something generally useful */
		/* note that TASK(USB_MainTask) will be periodically executed when no other tasks or functions are running */
     
    Endpoint_SelectEndpoint(IN_EP);

    if (dataToHostSize && Endpoint_ReadWriteAllowed())
    {
      if(dataToHostSize)
        Endpoint_Write_Stream_LE(dataToHost,dataToHostSize);
      
      /* Handshake the IN Endpoint - send the data to the host */
      Endpoint_ClearCurrentBank();
      
      dataToHostSize=0;
    }

    Endpoint_SelectEndpoint(OUT_EP);

    if (Endpoint_ReadWriteAllowed())
    {
      if( (dataFromHostSize=Endpoint_BytesInEndpoint()) >0 )
      {
        Endpoint_Read_Stream_LE(dataFromHost,dataFromHostSize);
        /* Clear the endpoint buffer */
        Endpoint_ClearCurrentBank();
        
        //first byte is always the command
        dataFromHostSize--;
        
        dataToHostSize=0;
        
        switch( dataFromHost[0] &JTAG_CMD_MASK ) 
        {
          
        case JTAG_CMD_TAP_OUTPUT:
          
          dataFromHostSize*=4;

          if( dataFromHost[0] & JTAG_DATA_MASK )
            dataFromHostSize-= (4- ((dataFromHost[0] & JTAG_DATA_MASK)>>4));
          
          dataToHostSize= jtag_tap_output( &dataFromHost[1] , dataFromHostSize, dataToHost);
          
          break;
          
        case JTAG_CMD_TAP_OUTPUT_EMU:
          dataFromHostSize*=4;
          if(dataFromHost[0]&JTAG_DATA_MASK)
            dataFromHostSize-=(4- ((dataFromHost[0]&JTAG_DATA_MASK)>>4));
          
          dataToHostSize=jtag_tap_output_emu(&dataFromHost[1], dataFromHostSize, dataToHost);
          
          break;
          
        case JTAG_CMD_READ_INPUT:
          dataToHost[0]=jtag_read_input();
          dataToHostSize=1;
          break;
        
        case JTAG_CMD_SET_SRST:
          jtag_set_srst(dataFromHost[1]&1);
          dataToHost[0]=0;//TODO: what to output here?
          dataToHostSize=1;
          break;
        
        case JTAG_CMD_SET_TRST:
          jtag_set_trst(dataFromHost[1]&1);
          dataToHost[0]=0;//TODO: what to output here?
          dataToHostSize=1;
          break;
        
        case JTAG_CMD_SET_DELAY:
          jtag_delay=dataFromHost[1];
          dataToHost[0]=0;//TODO: what to output here?
          dataToHostSize=1;
        
        default: //REPORT ERROR?
          break;
        }
      }
    }
	}
}
