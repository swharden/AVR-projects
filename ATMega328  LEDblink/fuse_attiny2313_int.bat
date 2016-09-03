@echo off
avrdude -c usbtiny -p m328p -U lfuse:w:0xe2:m -U hfuse:w:0xd9:m
pause