@echo off
avrdude -c usbtiny -p t2313 -U lfuse:w:0xe0:m -U hfuse:w:0xcf:m -U efuse:w:0xff:m
pause