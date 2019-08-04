DEL main.elf
DEL main.hex
avr-gcc -mmcu=atmega328p -std=c99 -Wall -Os -o main.elf main.c -w
avr-objcopy -j .text -j .data -O ihex main.elf main.hex