/*
simple case PWM on an ATTiny85.
Output is on PB4 (pin 3).
*/

#define F_CPU 8000000UL // clock frequency
#include <avr/io.h>
#include <util/delay.h>

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

int main() {
	setupPWM();
	for(;;){
		OCR1B=100;
		_delay_ms(1000);
		OCR1B=200;
		_delay_ms(1000);
	}
}
