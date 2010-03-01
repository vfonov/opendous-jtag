#include <avr/io.h>
#include <util/delay_basic.h>

#include "Descriptors.h"
#ifdef DEBUG
#include <stdio.h>
#endif //DEBUG

#include "jtag_defs.h"
#include "jtag_functions.h"

uint16_t jtag_delay=0;


//! initialize JTAG interface
void jtag_init(void)
{
  JTAG_OUT=0;
  JTAG_DIR=JTAG_OUTPUT_MASK; 
  JTAG_OUT=(1<<JTAG_PIN_TRST)|(1<<JTAG_PIN_SRST); //passive state high
/*  while(1)
  {
    asm("nop");
  }*/
}

//! send taps through JTAG interface and recieve responce from TDO pin only
//! \parameter out_buffer - buffer of taps for output, data is packed TDI and TMS values a stored together 
//! \parameter out_length - total number of pairs to send (maximum length is 4*255 samples)
//! \parameter in_buffer  - buffer which will hold recieved data data will be packed 
//! \return    number of bytes used in the in_buffer 
uint16_t jtag_tap_output_max_speed(const uint8_t *out_buffer, uint16_t out_length, uint8_t *in_buffer)
{
  uint8_t tms_tdi;
  uint8_t out_data;
  uint8_t in_data = 0;
  uint16_t out_buffer_index = 0; //output byte counter
  uint16_t out_length_index = 0; //output tms/tdo counter
  uint16_t in_buffer_index  = 0; //input  byte counter
	//cli();
#ifdef      DEBUG
  printf("jtag_tap_output_max_speed:Sending %d bits \r\n", out_length);
#endif

  while(1)
  {
    out_data = out_buffer[out_buffer_index++];

    //First TMS/TDI/TDO
    tms_tdi = out_data & JTAG_SIGNAL_MASK;
    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) ) | tms_tdi;
    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    asm("nop");//asm("nop");asm("nop");
    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    
    in_data >>= 1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_TDO))&0x80);

    out_length_index++;

    if(out_length_index>=out_length)
      break;
    
    //Second TMS/TDI/TDO
    out_data >>= 2;
    tms_tdi = out_data & JTAG_SIGNAL_MASK;
    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) ) | tms_tdi;
    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    asm("nop");//asm("nop");asm("nop");
    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    
    in_data >>= 1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_TDO))&0x80);

    out_length_index++;

    if(out_length_index>=out_length)
      break;
    
    //Third TMS/TDI/TDO
    out_data >>= 2;
    tms_tdi = out_data & JTAG_SIGNAL_MASK;
    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) ) | tms_tdi;
    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    asm("nop");//asm("nop");asm("nop");
    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    
    in_data >>= 1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_TDO))&0x80);

    out_length_index++;

    if(out_length_index>=out_length)
      break;

    //Fourth TMS/TDI/TDO
    out_data >>= 2;
    tms_tdi = out_data & JTAG_SIGNAL_MASK;
    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) ) | tms_tdi;
    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    asm("nop");//asm("nop");asm("nop");
    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    
    in_data >>= 1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_TDO))&0x80);

    out_length_index++;    

    if(!(out_length_index & 0x7)) //check if we fiilled the byte
    {
      in_buffer[in_buffer_index++] = in_data;
      in_data = 0;
    }

    if(out_length_index>=out_length)
      break;
  }

  if(out_length_index&0x7)//leftover 
    in_buffer[in_buffer_index++] = in_data>>(8-(out_length_index&0x7));

  //sei();

  return in_buffer_index;
}

//! send taps through JTAG interface and recieve responce from TDO pin only
//! \parameter out_buffer - buffer of taps for output, data is packed TDI and TMS values a stored together 
//! \parameter out_length - total number of pairs to send (maximum length is 4*255 samples)
//! \parameter in_buffer  - buffer which will hold recieved data data will be packed 
//! \return    number of bytes used in the in_buffer 
uint16_t jtag_tap_output_with_delay(const uint8_t *out_buffer, uint16_t out_length, uint8_t *in_buffer)
{
  uint8_t tms_tdi;
  uint8_t out_data;
  uint8_t in_data = 0;
  uint16_t out_buffer_index = 0;
  uint16_t in_buffer_index = 0;
  uint16_t out_length_index = 0;

#ifdef      DEBUG
  printf("jtag_tap_output_with_delay:Sending %d bits \r\n", out_length);
#endif

  while(1)
  {
    out_data = out_buffer[out_buffer_index++];

    //First TMS/TDI/TDO
    tms_tdi = out_data & JTAG_SIGNAL_MASK;
    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) ) | tms_tdi;
    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    _delay_loop_2(jtag_delay);
    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    _delay_loop_2(jtag_delay);
    
    in_data = in_data>>1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_TDO))&0x80);
    out_length_index++;
    if(out_length_index>=out_length)
      break;
    
    //Second TMS/TDI/TDO
    out_data = out_data>>2;
    tms_tdi = out_data & JTAG_SIGNAL_MASK;
    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) ) | tms_tdi;
    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    _delay_loop_2(jtag_delay);
    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    _delay_loop_2(jtag_delay);
    
    in_data = in_data>>1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_TDO))&0x80);
    out_length_index++;
    if(out_length_index>=out_length)
      break;
    
    //Third TMS/TDI/TDO
    out_data = out_data>>2;
    tms_tdi = out_data & JTAG_SIGNAL_MASK;
    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) ) | tms_tdi;
    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    _delay_loop_2(jtag_delay);
    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    _delay_loop_2(jtag_delay);
    
    in_data = in_data>>1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_TDO))&0x80);
    out_length_index++;
    if(out_length_index>=out_length)
      break;

    //Fourth TMS/TDI/TDO
    out_data = out_data>>2;
    tms_tdi = out_data & JTAG_SIGNAL_MASK;
    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) ) | tms_tdi;
    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    _delay_loop_2(jtag_delay);
    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    _delay_loop_2(jtag_delay);
    
    in_data = in_data>>1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_TDO))&0x80);
    out_length_index++;    
    if(!(out_length_index%8))
    {
      in_buffer[in_buffer_index] = in_data;
      in_buffer_index++;
      in_data = 0;
    }    
    if(out_length_index>=out_length)
      break;
  }

  if(out_length_index%8)
    in_buffer[in_buffer_index] = in_data>>(8-(out_length_index%8));
  
  return (out_length+7)/8;
}


//! send taps through JTAG interface and recieve responce from TDO and EMU pins 
//! \parameter out_buffer - buffer of taps for output, data is packed TDI and TMS values a stored together 
//! \parameter out_length - total number of pairs to send (maximum length is 4*255 samples)
//! \parameter in_buffer  - buffer which will hold recieved data data will be packed 
//! \return    number of bytes used in the in_buffer (equal to the input (length+3)/4
uint16_t jtag_tap_output_emu(const uint8_t *out_buffer,uint16_t out_length,uint8_t *in_buffer)
{
  uint8_t tms_tdi;
  uint8_t out_data;
  uint8_t in_data = 0;
  uint16_t out_buffer_index = 0;
  uint16_t in_buffer_index = 0;
  uint16_t out_length_index = 0;

  while(1)
  {
    out_data = out_buffer[out_buffer_index++];

    //First TMS/TDI/TDO
    tms_tdi = out_data & JTAG_SIGNAL_MASK;
    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) ) | tms_tdi;
    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    _delay_loop_2(jtag_delay);
    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    _delay_loop_2(jtag_delay);
    
    in_data = in_data>>1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_TDO))&0x80);
    in_data = in_data>>1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_EMU))&0x80);
    out_length_index++;
    if(out_length_index>=out_length)
      break;
    
    //Second TMS/TDI/TDO
    out_data = out_data>>2;
    tms_tdi = out_data & JTAG_SIGNAL_MASK;
    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) ) | tms_tdi;
    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    _delay_loop_2(jtag_delay);
    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    _delay_loop_2(jtag_delay);
    
    in_data = in_data>>1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_TDO))&0x80);
    in_data = in_data>>1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_EMU))&0x80);
    out_length_index++;
    if(out_length_index>=out_length)
      break;
    
    //Third TMS/TDI/TDO
    out_data = out_data>>2;
    tms_tdi = out_data & JTAG_SIGNAL_MASK;
    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) ) | tms_tdi;
    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    _delay_loop_2(jtag_delay);
    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    _delay_loop_2(jtag_delay);
    
    in_data = in_data>>1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_TDO))&0x80);
    in_data = in_data>>1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_EMU))&0x80);
    out_length_index++;
    if(out_length_index>=out_length)
      break;

    //Fourth TMS/TDI/TDO
    out_data = out_data>>2;
    tms_tdi = out_data & JTAG_SIGNAL_MASK;
    JTAG_OUT = ( JTAG_OUT & ( ~JTAG_SIGNAL_MASK ) ) | tms_tdi;
    JTAG_OUT|=JTAG_CLK_HI;//CLK hi
    _delay_loop_2(jtag_delay);
    JTAG_OUT&=JTAG_CLK_LO;//CLK lo
    _delay_loop_2(jtag_delay);
    
    in_data = in_data>>1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_TDO))&0x80);
    in_data = in_data>>1;
    in_data |= ((JTAG_IN<<(7-JTAG_PIN_EMU))&0x80);
    out_length_index++;    
    if(!(out_length_index%4))
    {
      in_buffer[in_buffer_index] = in_data;
      in_buffer_index++;
      in_data = 0;
    }    
    if(out_length_index>=out_length)
      break;
  }

  if(out_length_index%4)
    in_buffer[in_buffer_index] = in_data>>(8-2*(out_length_index%4));
  
  return (out_length+3)/4;
}

//! return current status of TDO & EMU pins
//! \return packed result TDO - bit 0 , EMU bit 1
uint16_t jtag_read_input(void)
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
	uint8_t mask=(trst<<JTAG_PIN_TRST)|(srst<<JTAG_PIN_SRST);
	uint8_t out=JTAG_OUT& (~ ((1<<JTAG_PIN_SRST)|(1<<JTAG_PIN_TRST)));

  JTAG_OUT=out|mask;
}


opendous_jtag_config_t PROGMEM opendous_jtag_config =
{
				.version_major=0,
				.version_minor=4,
				.max_tap_length=OPENDOUS_USB_BUFFER_SIZE*4 
};

uint16_t jtag_read_config(uint8_t *in_buffer)
{	
	//*((opendous_jtag_config*)(in_buffer))= 

	memcpy_P(in_buffer,&opendous_jtag_config,sizeof(opendous_jtag_config));
	return sizeof(opendous_jtag_config);
}
