@echo off
del *.elf
del *.hex
avr-gcc -mmcu=attiny85 -Wall -Os -o main.elf main.c
avr-objcopy -j .text -j .data -O ihex main.elf main.hex
avrdude -c buspirate -p attiny85 -P com3 -e -U flash:w:main.hex
python up.py