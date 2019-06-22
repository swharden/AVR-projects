::  microcontroller: ATMega328P
::     clock source: external crystal (>8MHz, 65ms startup time)
::     clock output: disabled
avrdude -c usbtiny -p m328p -U lfuse:w:0xff:m -U hfuse:w:0xd9:m