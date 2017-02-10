@echo off
del *.elf
del *.hex
avr-gcc -mmcu=atmega328p -Wall -Os -o main.elf main.c i2c_master.c -w -std=c99
avr-objcopy -j .text -j .data -O ihex main.elf main.hex
pause
avrdude -c usbtiny -p m328p -U flash:w:"main.hex":a 
pause