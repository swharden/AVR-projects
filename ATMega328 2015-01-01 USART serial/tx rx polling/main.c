#define F_CPU 1000000ul
#include <avr/io.h>
#include <util/delay.h>

#define USART_BAUDRATE 4800
#define UBRR_VALUE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

void serial_init(){
	// initialize USART (must call this before using it)
	UBRR0=UBRR_VALUE; // set baud rate
	UCSR0B|=(1<<TXEN0); //enable TX
	UCSR0B|=(1<<RXEN0); //enable RX
	UCSR0C|=(1<<UCSZ01)|(1<<UCSZ01); // no parity, 1 stop bit, 8-bit data
}

void serial_send(unsigned char data){
	// send a single character via USART
	while(!(UCSR0A&(1<<UDRE0))){}; //wait while previous byte is completed
	UDR0 = data; // Transmit data
}

char USARTReadChar()
{
	// wait until single character is read via USART
   while(!(UCSR0A & (1<<RXC0))){} // wait until data comes
   return UDR0;
}
	
int main(void){	
	serial_init();	
	char i='?';
	char input;
	for(;;){
		input=USARTReadChar(); // capture an input character (stalls until given)
		if ((input>='0')&&(input<='9')){i=input;} // if the input is a number, set it
		serial_send(i); // show the currently set value
	}
}