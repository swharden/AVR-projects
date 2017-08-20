#define F_CPU 11059200
#include <avr/io.h>
#include <util/delay.h>

void LED_ON(){PORTB|=(1<<PB1);}
void LED_OFF(){PORTB&=~(1<<PB1);}

void I1_ON(){PORTD|=(1<<PD7);}
void I1_OFF(){PORTD&=~(1<<PD7);}

void I2_ON(){PORTD|=(1<<PD6);}
void I2_OFF(){PORTD&=~(1<<PD6);}

void fire(){
	LED_ON(); // indicate we are busy firing
	
	// excitatory slope
	I1_ON();
	_delay_ms(2);
	I1_OFF();
	_delay_us(500);	
	
	// inhibitory slope
	I2_OFF();
	_delay_ms(2);
	//_delay_us(100);	// extra AHP
	
	// return to original conditions
	I2_ON();
	
	// wrap up
	_delay_ms(30); // give some time before returning / resetting
	LED_OFF(); // indicate we are ready to fire again
}

void init_ADC(){	
	// make ADC run continuously (free running mode)
	//ADMUX|=_BV(REFS1)|_BV(REFS0); // internal 1.1V reference with cap on AREF pin (leave blank for AREF)
	ADMUX|=0b0101; // read from ADC5 (PC5, pin 28)
	ADCSRA|=_BV(ADEN);// ADC enable
	ADCSRA|=_BV(ADPS1)|_BV(ADPS0); // prescale by 8	(make sure CK/prescale is 50kHz-200kHz)
	ADCSRA |= _BV(ADATE); // ADC auto trigger enable (enable for free running mode)
	ADCSRA |= _BV(ADSC); // start the first conversion, and go forever if free running mode
}

void sleepMS(int time){
	while(time--) {_delay_ms(1);}
}

int main(void){
	DDRB|=(1<<PB1); // LED
	DDRD|=(1<<PD6)|(1<<PD7); // fast currents
	
	init_ADC();
	int i;
	
	
	for(;;){
		fire();
		sleepMS((rand()%500)*ADC/100); // random based on ADC
		//sleepMS(100); // constant
		//sleepMS(ADC); // regular, based on ADC
	}
}
