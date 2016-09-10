/*
This program interfacs an ATMega328p with an SN74LV8154N (32-bit counter). 
No interrupts are being used here. We expect the RCLK pin to handle the gating.
Output is [count, difference] via serial protocol. 19200 baud, 8-bit, no parity.
*/

#define F_CPU 11059200ul
#include <avr/io.h>
#include <util/delay.h>

#define USART_BAUDRATE 19200
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

void serial_string(const char* s){
    while(*s){
		serial_send(*s++);
	}
}

void serial_break(){
	serial_send(10); // new line 
	serial_send(13); // carriage return
}
void serial_comma(){
	serial_send(','); // comma
	serial_send(' '); // space
}

void serial_number(uint32_t val){ // send a number as ASCII text
	uint32_t divby=1000000000; // change by dataType
	while (divby){
		serial_send('0'+val/divby);
		val-=(val/divby)*divby;
		divby/=10;
	}
}

void serial_binary(int val){ // send a number as ASCII text
	char bitPos;
	for(bitPos=8;bitPos;bitPos--){
		if ((val>>(bitPos-1))&1){serial_send('1');}
		else {serial_send('0');}
	}
}
	
void serial_test(){
	char i;
	serial_break();
	for(i=65;i<65+26;i++){
		serial_send(i);
	}
	serial_break();
}

int main(void){	
	serial_init();	
	uint32_t i=9999990;
	uint32_t j=3222111000;
	char times;
	for(;;){
		i++;
		j++;
		serial_number(i);
		serial_comma();
		serial_number(j);
		serial_break();
		_delay_ms(500);
	}
}