#define	F_CPU (11059200UL)
#include <avr/io.h>
#include <util/delay.h>

int main (void){	
    DDRB=(1<<PB0); // TTL output
	PORTB=0; // internal pull-down
	while(1){
		while((PINB&(1<<PB2))==0){} // hang while LOW
		PORTB=(1<<PB0); // TTL ON
		_delay_ms(5);
		PORTB&=~(1<<PB0); // TTL OFF
		_delay_ms(20);
	}
}