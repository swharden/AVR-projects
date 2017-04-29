@echo off
del *.elf
del *.hex
avr-gcc -mmcu=atmega328p -Wall -Os -DF_CPU=8000000UL -o main.elf main.c uart/uart.c pcf8574/pcf8574.c lcdpcf8574/lcdpcf8574.c i2chw/twimaster.c -w
avr-objcopy -j .text -j .data -O ihex main.elf main.hex
pause
avrdude -c usbtiny -p m328p -U flash:w:"main.hex":a 
pause