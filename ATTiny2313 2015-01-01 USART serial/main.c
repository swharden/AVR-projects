#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

#define USART_BAUDRATE 4800
#define UBRR_VALUE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

void serial_init(){
	// initialize USART
	UBRRL = UBRR_VALUE & 255; 
	UBRRH = UBRR_VALUE >> 8;
	UCSRB = (1 << TXEN); // fire-up USART
	UCSRC = (1 << UCSZ1) | (1 << UCSZ0); // fire-up USART
}

void serial_send(unsigned char data){
	// send a single character via USART
	while(!(UCSRA&(1<<UDRE))){}; //wait while previous byte is completed
	UDR = data; // Transmit data
}

void serial_break(){
	serial_send(10); // new line 
	serial_send(13); // carriage return
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

int main(void){
	serial_init();
	int i;
	// show a test alphabet and some numbers
	for(;;){
		i++;
		serial_send('a'+i%26);
		if ((i%26)==25){
			serial_number(i);
			serial_break();
			_delay_ms(100);
		}
	}
}
