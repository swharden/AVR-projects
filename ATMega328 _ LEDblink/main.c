#define F_CPU 11059200
#include <avr/io.h>
#include <util/delay.h>

int main(void){
	DDRB|=(1<<PB1); // set PD6 (pin 11) output
	for(;;){
		PORTB|=(1<<PB1); // set PD
		_delay_ms(200); // wait
		PORTB&=~(1<<PB1);// set PD6 low
		_delay_ms(800); // wait
	}
}
