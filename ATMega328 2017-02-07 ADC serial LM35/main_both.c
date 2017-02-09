/*
This code reads ADC of PC5 (pin 28) and reports its value
as a 10-bit number over serial TX pin (pin 3).

Serial settings require fuse set to internal oscillator 
and div/8 so internal clock is 1MHz, yielding 2400 baud.

Free running mode is used.

This was tested for ATMega328
*/

#define F_CPU 1000000ul
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#define USART_BAUDRATE 2400
#define UBRR_VALUE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

void serial_init(){
	// initialize USART (must call this before using it)
	UBRR0=UBRR_VALUE; // set baud rate
	UCSR0B|=(1<<TXEN0); //enable transmission only
	UCSR0C|=(1<<UCSZ01)|(1<<UCSZ01); // no parity, 1 stop bit, 8-bit data
}

void serial_send(unsigned char data){
	// send a single character via USART
	while(!(UCSR0A&(1<<UDRE0))){}; //wait while previous byte is completed
	UDR0 = data; // Transmit data
}

void serial_break(){
	serial_send(10); // new line 
	//serial_send(13); // carriage return
}

void serial_comma(){
	serial_send(','); // comma
	serial_send(' '); // space
}

void serial_number(long val){
	// send a number as ASCII text
	long divby=100000000; // change by dataType
	while (divby>=1){
		serial_send('0'+val/divby);
		val-=(val/divby)*divby;
		divby/=10;
	}
}

int read_ADC(){
	// return a single ADC reading
	// free running mode shouldnt be enabled (ADATE)
	ADCSRA |= (1<<ADSC); // start the first conversion, and go forever if free running mode
	while (ADCSRA & (1<<ADSC)) {}; // wait for measurement to finish
	return ADC;
}

int read_ADC10(){
	// return the sum of ten reads
	int i=0;
	int val=0;
	for (i=0;i<10;i++){
		val+=read_ADC();
	}
	return val/10;
}

void init_ADC(){	
	// prepare the ADC to run in ADC Noise Reduction mode (automatically when sleep happens)
	ADMUX|=_BV(REFS1)|_BV(REFS0); // internal 1.1V reference with cap on AREF pin (leave blank for AREF)
	ADMUX|=0b0101; // read from ADC5 (PC5, pin 28)
	ADCSRA|=_BV(ADEN);// ADC enable
	ADCSRA|=_BV(ADIE); // when sei(), ADC conversion complete interrupt is activated
	ADCSRA|=_BV(ADPS1)|_BV(ADPS0); // prescale by 8	(make sure CK/prescale is 50kHz-200kHz)
}

ISR(ADC_vect){} // called when the ADC conversion completes in sleep. Absolutely Must be here!

int ADC_read_quiet(){
	// enter sleep mode, make an ADC read, wake up, and return the read.
    set_sleep_mode(SLEEP_MODE_ADC); // set sleep mode to this
    sleep_enable(); // enable sleep
    sleep_mode(); // run the sleep mode (this starts an ADC read like ADCSRA |= (1<<ADSC) does)
	// ...when conversion is complete, ADC_vect is called, then we wake up and continue...
    sleep_disable(); // disable sleep
	return ADC;
}

int ADC_read_quiet10(){
	int adcTot=0;
	int i;
	for(i=0;i<10;i++){
		adcTot+=ADC_read_quiet();
	}
	return adcTot;
}

int ADC_read(){
	// return an ADC read made not using noise reduction
	ADCSRA |= (1<<ADSC); // tell it to manually start a read
	while (ADCSRA & (1<<ADSC)) {}; // wait for measurement to finish
	return ADC;
}

int ADC_read10(){
	int adcTot=0;
	int i;
	for(i=0;i<10;i++){
		adcTot+=ADC_read();
	}
	return adcTot;
}

int main(void){
	serial_init();
	sei();
	init_ADC();
	for(;;){
		_delay_ms(200);
		serial_number(ADC_read_quiet());
		serial_comma();
		serial_number(ADC_read());
		serial_break();				
	}	
}