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

void squares(int count, int low, int high){
	while(count--){
		OCR1B=low;
		_delay_ms(500);
		OCR1B=high;
		_delay_ms(500);
	}
}

void triangle(int count, int low, int high){
	while(count--){
		OCR1B=low;
		while(OCR1B<high){OCR1B++;_delay_ms(5);}
		while(OCR1B>low){OCR1B--;_delay_ms(5);}
	}
}

int main() {
	setupPWM();
	for(;;){
		squares(3,50,200);
		squares(3,150,200);
		squares(3,0,255);
		triangle(3,50,100);
		triangle(3,10,200);
		triangle(3,0,255);
	}
}