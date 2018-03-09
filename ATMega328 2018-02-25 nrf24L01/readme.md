# Getting nrf24l01 / nrf24l01+ Running on ATMega328P
I got a lot of benefit from [kehribar's nrf24L01_plus project](kehribar/nrf24L01_plus) and made a boilerplate project to allow 2 ATMega328P chips to communicate bidirectionally each with their own radio. This method is a little slow (it doesn't use interrupts) but it's so convenient that I love it for simple projects.

## Wiring
_Don't forget that the nrf24L01 requires 3.3 Volts, not 5 Volts!_

nrf24L01 | ATMega328
--- | ---
CE | PD0
CSN | PD1
SCK | PD2
MOSI | PD3
MISO | PD4

## Example Code
This project uses 2 microcontrollers (TX and RX). A 3-byte code is sent and controls the on/off state of 3 LEDs (red, green, blue) on the RX. The TX knows if the RX received the message and lights-up a green or red LED to indicate this.

* [transmitter code](src/main_tx.c)
* [receiver code](src/main_rx.c)

## Links
* https://github.com/kehribar/nrf24L01_plus
* http://www.tinkerer.eu/AVRLib/nRF24L01/
* http://gizmosnack.blogspot.com/2013/04/tutorial-nrf24l01-and-avr.html
* https://github.com/antoineleclair/avr-nrf24l01
* https://github.com/kehribar/nrf24L01_plus
* https://www.insidegadgets.com/2012/08/22/using-the-nrf24l01-wireless-module/
* https://www.theengineeringprojects.com/2015/07/interfacing-arduino-nrf24l01.html
* http://arduinotehniq.blogspot.com/2016/01/nrf24l01-radio-module-and-arduino.html
* http://www.deviceplus.com/how-tos/arduino-guide/nrf24l01-rf-module-tutorial/
* http://forum.arduino.cc/index.php?topic=290701.0
* http://www.tinkerer.eu/AVRLib/nRF24L01/