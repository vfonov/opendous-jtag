#ifndef __JTAG_DEFS_H__
#define __JTAG_DEFS_H__

	//jtag i/o pins
	#define JTAG_OUT PORTB
	#define JTAG_IN  PINB
	#define JTAG_DIR DDRB

	//ouput pins
	#define JTAG_PIN_TDI  0
	#define JTAG_PIN_TMS  1
	#define JTAG_PIN_TRST 2
	#define JTAG_PIN_SRST 3
	#define JTAG_PIN_TCK  4
	//input pins
	#define JTAG_PIN_TDO  5
	#define JTAG_PIN_EMU  6
	#define JTAG_PIN_RTCK 5

	//JTAG usb commands
	//TODO: ADD commands to deal with RTCK ?
	//TODO: maybe add commands to query some firmware info...
	#define JTAG_CMD_TAP_OUTPUT     0x0
	#define JTAG_CMD_SET_TRST       0x1
	#define JTAG_CMD_SET_SRST       0x2
	#define JTAG_CMD_READ_INPUT     0x3
	#define JTAG_CMD_TAP_OUTPUT_EMU 0x4
	#define JTAG_CMD_SET_DELAY      0x5
	#define JTAG_CMD_SET_SRST_TRST  0x6

	//JTAG usb command mask
	#define JTAG_CMD_MASK       0x0f
	#define JTAG_DATA_MASK      0xf0

	//JTAG pins masks
	#define JTAG_OUTPUT_MASK ((1<<JTAG_PIN_TDI)|(1<<JTAG_PIN_TMS)|(1<<JTAG_PIN_TRST)|(1<<JTAG_PIN_SRST)|(1<<JTAG_PIN_TCK))
	#define JTAG_INPUT_MASK  ((1<<JTAG_PIN_TDO)|(1<<JTAG_PIN_EMU)|(1<<JTAG_PIN_RTCK))
	#define JTAG_SIGNAL_MASK ((1<<JTAG_PIN_TDI)|(1<<JTAG_PIN_TMS))

	#define JTAG_CLK_LO  ~(1<<JTAG_PIN_TCK)
	#define JTAG_CLK_HI   (1<<JTAG_PIN_TCK)

	//additional delay to make clk hi and lo approximately the same length, not sure if this is really needed
	#define JTAG_DELAY2 20

#endif //__JTAG_DEFS_H__