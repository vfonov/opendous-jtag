#! /bin/sh

/opt/avr/bin/dfu-programmer atmega32u4 erase --debug 4
/opt/avr/bin/dfu-programmer atmega32u4 flash opendous-jtag.hex --debug 4
