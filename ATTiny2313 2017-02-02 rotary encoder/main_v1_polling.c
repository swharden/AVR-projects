#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

// R1 is PB2
// R2 is PB1
// B is PB0

void LED(int onTimeMS){
	// turn on the LED for a certain number of ms
	PORTD|=(1<<PD6); // LED ON
	while (onTimeMS--){_delay_ms(1);}
	PORTD&=~(1<<PD6); // LED OFF
}

void pollEncoder(){
	if (~PINB&(1<<PB2)) {
		// R1 went LOW, so see state of R2
		if(PINB&(1<<PB1)){
			// R2 is HIGH, so you're turning LEFT
			LED(10);
		} else {
			// R2 is LOW, so you're turning RIGHT
			LED(2);
		}	
	while (~PINB&(1<<PB2)){} // wait until R1 comes back high
	}
	
}

int main(void){
	DDRD|=(1<<PD6); // set PD6 (pin 11) output
	for(;;){
		pollEncoder();
	}
}
