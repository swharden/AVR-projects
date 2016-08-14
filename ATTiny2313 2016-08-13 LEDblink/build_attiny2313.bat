@echo off
del *.elf
del *.hex
avr-gcc -mmcu=attiny2313 -Wall -Os -o main.elf main.c -w
avr-objcopy -j .text -j .data -O ihex main.elf main.hex -v
pause