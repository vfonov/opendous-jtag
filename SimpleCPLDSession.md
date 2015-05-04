# Introduction #

This example was tested with Coolrunner-II CPLD from Xilinx ( XC2C256 )

# Details #

  * Download BSDL files from Xilinx ( http://www.xilinx.com/isp/bsdl/bsdl.htm )
  * Compile your project in Xilinx ICE, producing a .jed file
  * Run iMPACT and SVF file as a target
  * Program virtual device, creaing an svf file
  * Start urjtag and do following commands:
    * `cable opendous`
    * `bsdl path <where you have unpacked your bsdl files>`
    * `detect`
    * `svf <your svf file>`