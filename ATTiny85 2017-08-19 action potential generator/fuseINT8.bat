:: external 8+MHz crystal, 14ck+65ms startup time, ckout enabled
avrdude -c usbtiny -p attiny85 -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
pause