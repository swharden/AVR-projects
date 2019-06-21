:: ATMEGA328P COMPILE (AVG-GCC) AND PROGRAM (AVRDUDE/USBTINY) SCRIPT
:: by Scott Harden / Harden Technologies, LLC

@echo off

:: delete old files if they exist
ECHO | SET /p="deleting old files ... "
IF EXIST "main.elf" (
    DEL main.elf
)
IF EXIST "main.hex" (
    DEL main.hex
)
ECHO.

:: compile C code into a GNU binary file
avr-gcc -mmcu=atmega328p -Wall -Os -o main.elf main.c -w
ECHO | SET /p="compiling C file ... "
IF EXIST "main.elf" (
    ECHO.
) ELSE (
    ECHO FAILED!
    EXIT
)

:: convert the binary file into a hex file
ECHO | SET /p="creating HEX file ... "
avr-objcopy -j .text -j .data -O ihex main.elf main.hex
IF EXIST "main.hex" (
    ECHO.
) ELSE (
    ECHO FAILED!
    EXIT
)

:: copy the hex file onto the microcontroller using a USB programmer (-q?)
ECHO programming microcontroller ...
ECHO.
avrdude -c usbtiny -p m328p -U flash:w:"main.hex":a 