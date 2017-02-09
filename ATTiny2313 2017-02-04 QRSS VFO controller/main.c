#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* simple demonstration of how to use a rotary encoder with an ATTiny2313
 R1 (rotary encoder switch 1) is PB2
 R2 (rotary encoder switch 2) is PB1
 B (button) is PB0
 LED is on PB3 (PWM controlled output)
*/

volatile int offset=500;

void setupPWM_16bit(){
    DDRB|=(1<<PB3); // enable 16-bit PWM output on PB3
	TCCR1A|=(1<<COM1A1); // Clear OC1A/OC1B on Compare Match
	TCCR1B|=(1<<WGM13); // enable "PWM, phase and frequency correct"
	TCCR1B|=(1<<CS10); // enable output with the fastest clock (no prescaling)
	ICR1=10000; // set the top value (could be up to 2^16)
	OCR1A=5000; // set PWM pulse width (starts at 50% duty)
}

void duty_decrease(int dropBy){
	// darken the LED by decreasing PWM duty
	if (dropBy>OCR1A) {dropBy=OCR1A;}
	OCR1A-=dropBy;
}

void duty_increase(int raiseBy){
	// brighten the LED by increasing PWM duty
	if (OCR1A+raiseBy>ICR1) {raiseBy=ICR1-OCR1A;}
	OCR1A+=raiseBy;
}

void poll_encoder_v1(){
	// polls for turns only
	if (~PINB&(1<<PB2)) {
		if (~PINB&(1<<PB1)){
			// left turn
			duty_decrease(100);
		} else {
			// right turn
			duty_increase(100);
		}			
		_delay_ms(2); // force a little down time before continuing 
		while (~PINB&(1<<PB2)){} // wait until R1 comes back high
	}
}

void poll_encoder_v2(){
	// polls for turns as well as push+turns
	if (~PINB&(1<<PB2)) {
		if (~PINB&(1<<PB1)){
			if (PINB&(1<<PB0)){
				// left turn
				duty_decrease(100);
			} else {
				// left press and turn
				offset-=500;
			}
		} else {
			if (PINB&(1<<PB0)){
				// right turn
				duty_increase(100);
			} else {
				// right press and turn
				offset+=500;
			}
		}			
		_delay_ms(2); // force a little down time before continuing 
		while (~PINB&(1<<PB2)){} // wait until R1 comes back high
	}
}

// run this only when pin state changes
ISR(PCINT_vect){poll_encoder_v2();}

int main(void){
	
	setupPWM_16bit();
	
	// set up pin change interrupts
	GIMSK=(1<<PCIE); // Pin Change Interrupt Enable 
	EIFR=(1<<PCIF); // Pin Change Interrupt Flag
	PCMSK=(1<<PCINT1)|(1<<PCINT2)|(1<<PCINT3); // watch these pins
	sei(); // enable global interrupts
	
	for(;;){
		_delay_ms(500);
		duty_increase(offset);
		_delay_ms(500);
		duty_decrease(offset);
		
	}
}