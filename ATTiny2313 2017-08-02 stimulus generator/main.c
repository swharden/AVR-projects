#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>

volatile char state=0;

void output_HIGH(){PORTB|=(1<<PB4);}
void output_LOW(){PORTB&=~(1<<PB4);}
void LED1_ON(){PORTB|=(1<<PB3);}
void LED1_OFF(){PORTB&=~(1<<PB3);}
void LED2_ON(){PORTB|=(1<<PB2);}
void LED2_OFF(){PORTB&=~(1<<PB2);}

void singlePulse_20Hz_100us(){
	output_HIGH();
	_delay_us(100);
	output_LOW();
	_delay_us(900);
	LED2_ON();
	_delay_ms(20);
	LED2_OFF();
	_delay_ms(28);
	_delay_us(920);
}

void poll(){
	/*
	// check for front button press
	if (!(PIND&(1<<PD5))){PORTB=255;}
	else {PORTB=0;}
	*/
	
	//check for input signal
	if ((PIND&(1<<PD4))){singlePulse_20Hz_100us();}
}

int main(void){
	DDRB=255; // all outputs
	DDRD=0; // all inputs
	PORTD=(1<<PD5); // pull front button high
	LED1_ON();
	for(;;){
		poll();
	}
}