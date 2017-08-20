#define F_CPU 8000000UL // clock frequency
#include <avr/io.h>
#include <util/delay.h>

int main() {
	for(;;){
		PORTB|=(1<<PB4);
		_delay_ms(100);
		PORTB&=~(1<<PB4);
		_delay_ms(100);
	}
}
