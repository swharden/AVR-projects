# ATTiny85 PWM
Quick demo to get PWM up and running on an ATTiny85
- PWM outputs on PD4 (pin 3)
- [main.c](main.c) demonstrates how to manually set PWM value
- [main_squares_triangles.c](main_squares_triangles.c) draws squares and triangles

## Screenshot
![](pwm.jpeg)

## Important code:
```C
void setupPWM(){
	// Enable PLL (64 MHz)
	PLLCSR |= (1 << PLLE);
	
	// Use PLL as timer clock source
	PLLCSR |= (1 << PCKE);

	// Set prescaler to PCK (full speed ~248kHz @ 8MHz)
	TCCR1 |= (1 << CS10);

	// compare value and top value
	OCR1B = 0; // raise to increase PWM duty
	OCR1C = 255; // lower to increase PWM frequency

	// Enable OCRB output on PB4
	DDRB |= (1 << PB4);
	
	// toggle PB4 when when timer reaches OCR1B (target)
	GTCCR |= (1 << COM1B0); 
	
	// clear PB4 when when timer reaches OCR1C (top)
	GTCCR |= (1 << PWM1B);
	
	// sometimes required for old, glitchy chips.
	TCCR1 |= (1 << COM1A0);
}
```
