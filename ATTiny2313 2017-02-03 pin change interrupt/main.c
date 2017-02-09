#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// run this only when pin state changes
ISR(PCINT_vect){poll_encoder_v2();}

int main(void){
	
	// set up pin change interrupts
	GIMSK=(1<<PCIE); // Pin Change Interrupt Enable 
	EIFR=(1<<PCIF); // Pin Change Interrupt Flag
	PCMSK=(1<<PCINT1)|(1<<PCINT2)|(1<<PCINT3); // watch these pins
	sei(); // enable global interrupts
	
	for(;;){} //forever
}