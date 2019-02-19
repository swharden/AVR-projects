@echo off
avrdude -c usbtiny -p attiny85 -P com3 -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
pause