#define F_CPU 11059200
#include <avr/io.h>
#include <util/delay.h>

void LED_ON(){PORTB|=(1<<PB0);}
void LED_OFF(){PORTB&=~(1<<PB0);}

void I1_ON(){PORTB|=(1<<PB4);}
void I1_OFF(){PORTB&=~(1<<PB4);}

void I2_ON(){PORTB|=(1<<PB3);}
void I2_OFF(){PORTB&=~(1<<PB3);}

void fire(){
	LED_ON(); // indicate we are busy firing
	
	// rising part
	DDRB|=(1<<PB4);PORTB|=(1<<PB4); // make output and high
	_delay_ms(2);
	_delay_us(100);

	// pause at the top
	
	
	// falling part
	DDRB|=(1<<PB4);PORTB&=~(1<<PB4); // make output and low
	_delay_ms(2);
	_delay_us(150);
		
	// wrap up
	DDRB&=~(1<<PB4); // high impedance
	_delay_ms(50); // give some time before returning / resetting
	LED_OFF(); // indicate we are ready to fire again
}


void init_ADC(){	
	// make ADC run continuously (free running mode)
	ADMUX|=0b0001; // read from ADC1 (PB2, SCK, pin 7) [VCC is reference]
	ADCSRA|=_BV(ADEN);// ADC enable
	ADCSRA|=_BV(ADPS2)|_BV(ADPS1); // prescale by 64 (make sure CK/prescale is 50kHz-200kHz)
	ADCSRA |= _BV(ADATE); // ADC auto trigger enable (enable for free running mode)
	ADCSRA |= _BV(ADSC); // start the first conversion, and go forever if free running mode
}


void sleepMS(int time){
	while(time--) {_delay_ms(1);}
}

int main(void){
	DDRB|=(1<<PB0); // LED
	DDRB|=(1<<PB3)|(1<<PB4); // fast currents
	DDRB&=~(1<<PB1); // make this an input
	PORTB|=(1<<PB1); // pull the input high
	
	init_ADC();
	
	for(;;){
		fire();
		if(PINB&(1<<PB1)){
			sleepMS((rand()%500)*ADC/100); // random based on ADC
		} else {
			sleepMS(ADC);
		}
		
		//sleepMS(ADC); // regular, based on ADC
	}
}
