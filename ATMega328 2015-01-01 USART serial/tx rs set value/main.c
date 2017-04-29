#define F_CPU 1000000ul
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define USART_BAUDRATE 4800
#define UBRR_VALUE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

#define inputBufferLength 20 // number of characters the input buffer can be
volatile char input[inputBufferLength]; // input buffer

volatile int value=1234; // this will be changed based on terminal commands

// serial stuff

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

void serial_break(){
	serial_send(10); // new line 
	serial_send(13); // carriage return
}

void serial_long(long val){
	// send a number as ASCII text
	long divby=100000000;
	while (divby>=1){
		serial_send('0'+val/divby);
		val-=(val/divby)*divby;
		divby/=10;
	}
}

void serial_int(int val){
	// send a number as ASCII text
	int divby=1000;
	while (divby>=1){
		serial_send('0'+val/divby);
		val-=(val/divby)*divby;
		divby/=10;
	}
}

void serial_string(const char* s){
	// send a string of ASCII text
	while(*s){serial_send(*s++);}
}

// handle input buffer

ISR(USART_RX_vect){
	// you MUST read UDR0 inside this
	char new=UDR0;
	if (new=='\r'){
		input_leftShift();
		input_process();
	} else if (new=='\n'){
		// pass
	} else {
		input_add(new);
		serial_send(new);
	}
}

void input_add(char new){
	// add a new character to the input buffer
	char i;
	for(i=0;i<inputBufferLength-1;i++){
		input[i]=input[i+1];
	}
	input[inputBufferLength-1]=new;
}

void input_leftShift(){
	// remove all null values preceeding text
	char firstPos=0;
	char i;
	for(i=0;i<inputBufferLength;i++){
		if (input[i]>0){
			firstPos=i;
			break;
		}
	}
	for(i=0;i<inputBufferLength;i++){
		if (i>inputBufferLength-firstPos){
			input[i]=0;
		} else {
			input[i]=input[i+firstPos];
		}
	}
}

void input_reset(){
	// fill input array with null values
	char i;
	for(i=0;i<inputBufferLength;i++){
		input[i]=0;
	}
	serial_break();
	serial_string("listening:");
}

void input_process(){
	// display the contents of the input buffer 
	char i;
	serial_break();
	serial_string("captured:");
	for(i=0;i<inputBufferLength;i++){
		serial_send(input[i]);
	}
	serial_break();
	input_reset();
}

int main(void){	
	input_reset();
	serial_init();	
	sei();
	for(;;){
		//_delay_ms(1000);
		//input_show();
	}
}