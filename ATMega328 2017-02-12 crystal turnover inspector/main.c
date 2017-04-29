#define F_CPU 1000000ul
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "i2c_master.h"


#define F_CPU 1000000ul

#define USART_BAUDRATE 4800
#define UBRR_VALUE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

int readTemp(){
	// read LM75A sensor and return temperature as raw value
	// divide this value by 8 and you have temperature in C
	PORTC|=(1<<PC5)|(1<<PC4); // make sure pull-ups are on
	uint8_t data[2];
	uint8_t address=0b10010001; // 0b1001xxx1 where x is voltage on A2, A1, A0
	int temperature;
	i2c_receive(address,data,2);
	temperature=(data[0]*256+data[1])/32;
	return temperature;
}

int sendTemp(){
	// read temperature and send it as ASCII
	int temp;
	temp=readTemp();
	serial_number3(temp/8);
	temp-=8*(temp/8);
	temp*=125;
	serial_send('.');
	serial_number3(temp);
	serial_break();
}

void serial_break(){
	serial_send(10); // new line 
	//serial_send(13); // carriage return
}

void serial_init(){
	// initialize USART (must call this before using it)
	UBRR0=UBRR_VALUE; // set baud rate
	UCSR0B|=(1<<TXEN0); //enable TX
	UCSR0B|=(1<<RXEN0); //enable RX
	UCSR0B|=(1<<RXCIE0); //RX complete interrupt
	UCSR0C|=(1<<UCSZ01)|(1<<UCSZ01); // no parity, 1 stop bit, 8-bit data
}

void serial_number3(int val){
	// send a 3-digit number as ASCII text
	int divby=100; // change by dataType
	while (divby>=1){
		serial_send('0'+val/divby);
		val-=(val/divby)*divby;
		divby/=10;
	}
}

void serial_send(unsigned char data){
	// send a single character via USART
	while(!(UCSR0A&(1<<UDRE0))){}; //wait while previous byte is completed
	UDR0 = data; // Transmit data
}

volatile char action='?';
ISR(USART_RX_vect){
	_delay_ms(100);
	char new=UDR0;
	if (new=='r') {action='r';}
	else if (new=='o') {action='o';}
	else if (new=='f') {action='f';}
	else {action='x';}
}


int main(void){
	_delay_ms(200);
	serial_init();
	i2c_init();
	sei();
	serial_number3(123);
	serial_break();
	for(;;){
		_delay_ms(10);
		if (action=='r'){sendTemp();}
		else if (action=='o'){PORTD|=(1<<PD2);}
		else if (action=='f'){PORTD&=~(1<<PD2);}
		action='x';
	}
}