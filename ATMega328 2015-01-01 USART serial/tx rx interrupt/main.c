#define F_CPU 1000000ul
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define USART_BAUDRATE 4800
#define UBRR_VALUE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

void serial_init(){
	// initialize USART (must call this before using it)
	UBRR0=UBRR_VALUE; // set baud rate
	UCSR0B|=(1<<TXEN0); //enable TX
	UCSR0B|=(1<<RXEN0); //enable RX
	UCSR0B|=(1<<RXCIE0); //RX complete interrupt
	UCSR0C|=(1<<UCSZ01)|(1<<UCSZ01); // no parity, 1 stop bit, 8-bit data
}

void serial_send(unsigned char data){
	// send a single character via USART
	while(!(UCSR0A&(1<<UDRE0))){}; //wait while previous byte is completed
	UDR0 = data; // Transmit data
}

volatile char input='.';

ISR(USART_RX_vect){
	_delay_ms(100);
	char new=UDR0;
	if ((new>='0')&&(new<='9')){
		input=new;
	} else {
		input='?';
	}
}


int main(void){	
	serial_init();	
	sei();
	for(;;){
		_delay_ms(500);
		serial_send(input); // show the currently set value
	}
}