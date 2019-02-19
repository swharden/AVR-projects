@echo off
del *.elf
del *.hex
avr-gcc -mmcu=attiny85 -Wall -Os -o main.elf main.c
avr-objcopy -j .text -j .data -O ihex main.elf main.hex
pause
avrdude -c usbtiny -p attiny85 -U flash:w:"main.hex":a
pause