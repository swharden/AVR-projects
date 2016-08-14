# AVR clock divider
The purpose of this project is to use an external clock source, divide it by a certain number, and output the result on chip pins. 

The primary purpose of this project is to turn a 10MHz clock (from a canned oscillator) into 1Hz ticks.

## output types
* PB0 - "blip" 1Hz output at 5% duty (50 ms on, 950 ms off)
* PB1 - 10Hz pulses (bit flip at 20 Hz)
* PB3 - PWM output "pulses" strong/weak once per seond. PWM is ~50kHz
* PB4 - standard 1Hz output at 50% duty (500 ms on, 500 ms off)

![demo](demo.jpg)
