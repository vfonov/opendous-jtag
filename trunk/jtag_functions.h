#ifndef __JTAG_FUNCTIONS_H__
#define __JTAG_FUNCTIONS_H__

#include <stdint.h>
	//! initialize JTAG interface
	void jtag_init(void);

	//! send taps through JTAG interface and recieve responce from TDO pin only
	//! \parameter out_buffer - buffer of taps for output, data is packed TDI and TMS values a stored together 
	//! \parameter out_length - total number of pairs to send (maximum length is 4*255 samples)
	//! \parameter in_buffer  - buffer which will hold recieved data data will be packed 
	//! \return    number of bytes used in the in_buffer 
	uint8_t jtag_tap_output_max_speed(const uint8_t *out_buffer, uint16_t out_length, uint8_t *in_buffer);

	//! send taps through JTAG interface and recieve responce from TDO pin only
	//! \parameter out_buffer - buffer of taps for output, data is packed TDI and TMS values a stored together 
	//! \parameter out_length - total number of pairs to send (maximum length is 4*255 samples)
	//! \parameter in_buffer  - buffer which will hold recieved data data will be packed 
	//! \return    number of bytes used in the in_buffer 
	uint8_t jtag_tap_output_with_delay(const uint8_t *out_buffer, uint16_t out_length, uint8_t *in_buffer);

	//! send taps through JTAG interface and recieve responce from TDO and EMU pins 
	//! \parameter out_buffer - buffer of taps for output, data is packed TDI and TMS values a stored together 
	//! \parameter out_length - total number of pairs to send (maximum length is 4*255 samples)
	//! \parameter in_buffer  - buffer which will hold recieved data data will be packed 
	//! \return    number of bytes used in the in_buffer (equal to the input (length+3)/4
	uint8_t jtag_tap_output_emu(const uint8_t *out_buffer,uint16_t out_length,uint8_t *in_buffer);


	//! return current status of TDO & EMU pins
	//! \return packed result TDO - bit 0 , EMU bit 1
	uint8_t jtag_read_input(void);

	//! set pin TRST 
	void jtag_set_trst(uint8_t trst);


	//! set pin SRST 
	void jtag_set_srst(uint8_t srst);

	//! set both srst and trst simultaneously
	void jtag_set_trst_srst(uint8_t trst,uint8_t srst);

	extern uint16_t jtag_delay;

#endif //__JTAG_FUNCTIONS_H__