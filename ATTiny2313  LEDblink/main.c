#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void){
	DDRD|=(1<<PD6); // set PD6 (pin 11) output
	for(;;){
		_delay_ms(100); // wait
		PORTD|=(1<<PD6); // set PD
		_delay_ms(100); // wait
		PORTD&=~(1<<PD6);// set PD6 low
	}
}
