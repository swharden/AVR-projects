#define F_CPU 8000000UL // clock frequency
#include <avr/io.h>
#include <util/delay.h>

int main() {

	// set up the ADC to run continuously (free running mode)
	ADMUX|=0b00000010; // read from ADC1 (ADC2 (PB4), SCK, pin 7) [VCC is reference]
	ADCSRA|=_BV(ADEN);// ADC enable
	ADCSRA|=_BV(ADPS2)|_BV(ADPS1); // prescale by 64 (make sure CK/prescale is 50kHz-200kHz)
	ADCSRA |= _BV(ADATE); // ADC auto trigger enable (enable for free running mode)
	ADCSRA |= _BV(ADSC); // start the first conversion, and go forever if free running mode

	for(;;){ // do this forever
	
		// simulate a button press
		PORTB|=(1<<PB4); // pull high
		DDRB|=(1<<PB4); // make output
		_delay_ms(50); // hold it there
		
		// simulate a button release
		DDRB&=~(1<<PB4); // make input
		PORTB&=~(1<<PB4); // pull low
		_delay_ms(50); // hold it there
		
		// check if the button is actually pressed to togggle auto press
		if (ADC>200) { // if the button is manually pressed
			_delay_ms(100); // wait a bit
			while (ADC>200) {} // wait until it depresses
			while (ADC<200) {} // then wait for it to be pressed again
		}	
	}
}
