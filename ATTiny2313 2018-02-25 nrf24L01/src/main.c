#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void){
	DDRD|=(1<<PD6); // set PD6 (pin 11) output
	int i;
	for(i=0;i<100;i++){
		_delay_ms(100); // wait
		PORTD|=(1<<PD6); // set PD
		_delay_ms(100); // wait
		PORTD&=~(1<<PD6);// set PD6 low
	}
	for(;;){} // CKOUT works with minimal noise
}
