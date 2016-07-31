#define	F_CPU (1000000UL)
#include <avr/io.h>
#include <util/delay.h>

int main (void){	
    DDRB=255; // all outputs
	int counter=0;
	while(1){
		_delay_ms(1);
		counter+=1;
		if (counter>=1000) {counter=0;}
		if (counter%500==0) {PORTB^=(1<<PB0);} // 1Hz
		if (counter%250==0) {PORTB^=(1<<PB1);} // 2Hz
		if (counter%50==0) {PORTB^=(1<<PB2);} // 10Hz
		if (counter%10==0) {PORTB^=(1<<PB3);} // 50Hz
		PORTB^=(1<<PB4); // 1kHz
	}
}