del *.elf
del *.hex
avr-gcc -mmcu=atmega8 -Wall -Os -o main.elf main.c -w
pause
cls
avr-objcopy -j .text -j .data -O ihex main.elf main.hex

avrdude -c usbtiny -p m8 -F -U flash:w:"main.hex":a -U lfuse:w:0xFF:m -U hfuse:w:0xD9:m 


