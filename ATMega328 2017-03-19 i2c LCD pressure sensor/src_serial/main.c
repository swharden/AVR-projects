#define F_CPU 1000000ul
#include <avr/io.h>
#include <util/delay.h>

#define USART_BAUDRATE 4800
#define UBRR_VALUE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

#include "i2c_master.h"

void pressure(){
	PORTC|=(1<<PC5)|(1<<PC4); // make sure pull-ups are on
	uint8_t data[3];
	
	data[0]=72; // command 72 is "set register"
	i2c_transmit(0xEE,data,1); 
	
	_delay_ms(100);
	data[0]=0; // command 0 is "ADC read"
	i2c_transmit(0xEE,data,1); 
	
	// command 0 is "ADC read"
	i2c_receive(0xEF,data,3);
	
	unsigned long p=0;
	p+=data[0];
	p=p<<8;
	p+=data[1];
	p=p<<8;
	p+=data[2];
	
	// send result through serial
	serial_number(p);
	serial_break();
	_delay_ms(100);
}

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
	serial_send(13); // carriage return
}
void serial_comma(){
	serial_send(','); // comma
	serial_send(' '); // space
}

void serial_number(unsigned long val){ 
	// send a number as ASCII text
	char preVal=' ';
	unsigned long divby=1000000000; // change by dataType
	while (divby>=1){
		serial_send('0'+val/divby);
		val-=(val/divby)*divby;
		divby/=10;
	}
}

int main(void){
	serial_init();
	int i;
	for(;;){
		pressure();
		_delay_ms(500);
	}	
}