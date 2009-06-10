#include <avr/io.h>
#include <util/delay_basic.h>

#include "Descriptors.h"
#include "jtag_defs.h"
#include "jtag_functions.h"

uint16_t jtag_delay=0;


//! initialize JTAG interface
void jtag_init(void)
{
  JTAG_OUT=JTAG_PIN_TRST|JTAG_PIN_SRST; //passive state high
  JTAG_DIR=JTAG_OUTPUT_MASK; 
}

//! send taps through JTAG interface and recieve responce from TDO pin only
//! \parameter out_buffer - buffer of taps for output, data is packed TDI and TMS values a stored together 
//! \parameter out_length - total number of pairs to send (maximum length is 4*255 samples)
//! \parameter in_buffer  - buffer which will hold recieved data data will be packed 
//! \return    number of bytes used in the in_buffer 
uint8_t jtag_tap_output_max_speed(const uint8_t *out_buffer, uint16_t out_length, uint8_t *in_buffer)
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

    JTAG_OUT|=JTAG_CLK_HI;//CLK hi

    asm("nop");

    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    uint8_t data=JTAG_IN;
    
    if(!bit2)
      in_buffer[index2]=0;
    
    in_buffer[index2] |= ((data>>JTAG_PIN_TDO)&1)<<bit2;
  }
  
  return (out_length+7)/8;
}

//! send taps through JTAG interface and recieve responce from TDO pin only
//! \parameter out_buffer - buffer of taps for output, data is packed TDI and TMS values a stored together 
//! \parameter out_length - total number of pairs to send (maximum length is 4*255 samples)
//! \parameter in_buffer  - buffer which will hold recieved data data will be packed 
//! \return    number of bytes used in the in_buffer 
uint8_t jtag_tap_output_with_delay(const uint8_t *out_buffer, uint16_t out_length, uint8_t *in_buffer)
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

    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    _delay_loop_2(jtag_delay);

    JTAG_OUT&=JTAG_CLK_LO;//CLK lo

		_delay_loop_2(jtag_delay);
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

    if(jtag_delay>0) _delay_loop_2(jtag_delay);

    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    if(jtag_delay>0) _delay_loop_2(jtag_delay);


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

//! set both srst and trst simultaneously
void jtag_set_trst_srst(uint8_t trst,uint8_t srst)
{
  JTAG_OUT=(JTAG_OUT&(~ ((1<<JTAG_PIN_SRST)|(1<<JTAG_PIN_TRST)) ))| 
           (srst<<JTAG_PIN_SRST)|(trst<<JTAG_PIN_TRST);
}
