A simple USB-JTAG interface implemented using LUFA library running AT90USB162 based on Open Hardware board opendous.
Primary reason is to create a cheap replacement for the parallel port JTAG adapter "Wiggler" and alike.

### Connecting pins to JTAG interface ###
PORTB is used for JTAG communications, with following pins configuration:

|Pin|Function|
|:--|:-------|
|0 |TDI|
|1 |TMS|
|2 |TRST|
|3 |SRST|
|4 |TCK|
|5 |TDO|

Various JTAG cable pin outs are available here: http://www.jtagtest.com/pinouts/


### Configuring openocd ###
get the patched openocd-0.4.0:
```
wget http://opendous-jtag.googlecode.com/files/openocd-0.4.0.opendous.estick.tar.bz2
tar jxf openocd-0.4.0.opendous.estick.tar.bz2
```

Run the ./configure  --enable-opendous
or ./configure  --enable-estick
it is synonym
```
cd openocd-0.4.0.opendous.estick
./configure --prefix=/opt/arm --enable-opendous
make 
make install
```

make sure that PATH includes /opt/arm/bin directory, run openocd:
```
openocd -f interface/opendous -f target/<your cpu target>
```

### Testing flash write speed using LM3S2965 ###
Test of loading a eLua project ( http://www.eluaproject.net/en_downloads.html#binaries ) on a  Stellaris LM3S2965 demo kit, opendous-jtag was running on ATMEGA32U4 mcu, but using only 360 bytes memory buffer (similar to AT90USB162 setting)

```
> openocd -f interface/opendous.cfg -f target/lm3s6965.cfg
Open On-Chip Debugger 0.4.0 (2010-04-19-22:29)
Licensed under GNU GPL v2
For bug reports, read
	http://openocd.berlios.de/doc/doxygen/bugs.html
500 kHz
jtag_nsrst_delay: 100
jtag_ntrst_delay: 100
srst_only separate srst_gates_jtag srst_open_drain
Info : eStick JTAG Interface ready
Info : clock speed 500 kHz
Info : JTAG tap: lm3s6965.cpu tap/device found: 0x3ba00477 (mfg: 0x23b, part: 0xba00, ver: 0x3)
Info : lm3s6965.cpu: hardware has 6 breakpoints, 4 watchpoints
```
Now running command through telnet
```
telnete localhost 4444

> halt
target state: halted
target halted due to debug-request, current mode: Handler PendSV
xPSR: 0x6100000e pc: 0x0000b1ac msp: 0x20008390

> stellaris mass_erase 0
stellaris mass erase complete

> flash write_image elua0.7_lua_lm3s6965.bin 0 bin
not enough working area available(requested 8192, free 8152)
wrote 226136 bytes from file /home/vfonov/downloads/elua0.7_lua_lm3s6965.bin in 22.894466s (9.646 kb/s)

> reset
JTAG tap: lm3s6965.cpu tap/device found: 0x3ba00477 (mfg: 0x23b, part: 0xba00, ver: 0x3)
> shutdown
shutdown command invoked
```

Then trying to see if eLua is working:
```
$ picocom /dev/ttyUSB0 -b 115200
picocom v1.4

port is        : /dev/ttyUSB0
flowcontrol    : none
baudrate is    : 115200
parity is      : none
databits are   : 8
escape is      : C-a
noinit is      : no
noreset is     : no
nolock is      : no
send_cmd is    : ascii_xfr -s -v -l10
receive_cmd is : rz -vv

Terminal ready

eLua# help
Shell commands:
  exit        - exit from this shell
  help        - print this help
  ls or dir   - lists ROMFS files and sizes
  cat or type - lists file contents
  lua [args]  - run Lua with the given arguments
  recv        - receive a file (XMODEM) and execute it
  ver         - print eLua version
eLua# ver
eLua version 0.7
For more information go to http://www.eluaproject.net
eLua# lua /rom/info.lua
Press CTRL+Z to exit Lua
I'm running on platform LM3S
The CPU is a LM3S6965
The board name is EK-LM3S6965
```

### Testing memory write speed on Samsung s3c2440 on FriendlyARM board ###
Following http://labs.kernelconcepts.de/Publications/Micro2440/

```
openocd -f interface/opendous.cfg -f target/samsung_s3c2440.cfg
Open On-Chip Debugger 0.4.0 (2010-04-19-22:29)
Licensed under GNU GPL v2
For bug reports, read
	http://openocd.berlios.de/doc/doxygen/bugs.html
trst_and_srst separate srst_gates_jtag trst_push_pull srst_open_drain
Info : eStick JTAG Interface ready
Error: usb_bulk_read failed (requested=1, result=-110)
Error: Estick command 0x06 failed (-1)
Info : RCLK (adaptive clock speed)
Info : JTAG tap: s3c2440.cpu tap/device found: 0x0032409d (mfg: 0x04e, part: 0x0324, ver: 0x0)
Info : Embedded ICE version 2
Info : s3c2440.cpu: hardware has 2 breakpoint/watchpoint units
Info : accepting 'telnet' connection from 0
Info : JTAG tap: s3c2440.cpu tap/device found: 0x0032409d (mfg: 0x04e, part: 0x0324, ver: 0x0)
Warn : NOTE! DCC downloads have not been enabled, defaulting to slow memory writes. Type 'help dcc'.
Warn : NOTE! Severe performance degradation without fast memory access enabled. Type 'help fast'.
```
Now running command through telnet
```
$ telnet localhost 4444
> reset
JTAG tap: s3c2440.cpu tap/device found: 0x0032409d (mfg: 0x04e, part: 0x0324, ver: 0x0)
NOTE! DCC downloads have not been enabled, defaulting to slow memory writes. Type 'help dcc'.
NOTE! Severe performance degradation without fast memory access enabled. Type 'help fast'.
> reset halt
JTAG tap: s3c2440.cpu tap/device found: 0x0032409d (mfg: 0x04e, part: 0x0324, ver: 0x0)
target state: halted
target halted in ARM state due to debug-request, current mode: Supervisor
cpsr: 0x600000d3 pc: 0x00000000
MMU: disabled, D-Cache: disabled, I-Cache: disabled
NOTE! DCC downloads have not been enabled, defaulting to slow memory writes. Type 'help dcc'.
NOTE! Severe performance degradation without fast memory access enabled. Type 'help fast'.
> bp 0x33f80000 4 hw
breakpoint set at 0x33f80000
> resume
target state: halted
target halted in ARM state due to breakpoint, current mode: Supervisor
cpsr: 0x400000d3 pc: 0x00000000
MMU: disabled, D-Cache: disabled, I-Cache: disabled
> load_image /opt/FriendlyARM/friendlyarm2440-package/u-boot.bin 0x33f80000
239016 bytes written at address 0x33f80000
downloaded 239016 bytes in 36.938187s (6.319 kb/s)
> resume
```
Now switching to serial console:
```
$ picocom /dev/ttyS0 -b 115200
U-Boot 1.3.2-dirty-moko12 (Apr 16 2009 - 18:14:52)

I2C:   ready
DRAM:  64 MB
Flash:  2 MB
NAND:  Bad block table not found for chip 0
Bad block table not found for chip 0
128 MiB
*** Warning - bad CRC or NAND, using default environment

USB:   S3C2410 USB Deviced
In:    serial
Out:   serial
Err:   serial
MAC: 08:08:11:18:12:27
Hit any key to stop autoboot:  0
 
MINI2440 #
```
