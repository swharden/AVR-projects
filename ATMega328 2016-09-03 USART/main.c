#define F_CPU 11059200ul
#include <avr/io.h>
#include <util/delay.h>

#define USART_BAUDRATE 115200
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
	serial_send(13); // carriage return
}
void serial_comma(){
	serial_send(','); // comma
	serial_send(' '); // space
}

void serial_number(long val){ // send a number as ASCII text
	long divby=100000000; // change by dataType
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
		for(i='A';i<='Z';i++){serial_send(i);} // send the alphabet
		serial_break();
		
		serial_number(10140000+123); // send a big number
		serial_break();
		
		_delay_ms(1000); // wait a while
	}	
}