/*
This program interfacs an ATMega328p with an SN74LV8154N (32-bit counter). 
No interrupts are being used here. We expect the RCLK pin to handle the gating.
Output is [count, difference] via serial protocol. 19200 baud, 8-bit, no parity.
*/

#define F_CPU 1000000ul
#include <avr/io.h>
#include <util/delay.h>

#define USART_BAUDRATE 4800
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

void serial_number(unsigned long int val){ // send a number as ASCII text
	unsigned long int divby=1000000000; // change by dataType
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

int counter_reg(char reg){
	// set all registers high
	PORTD|=(1<<PD5)|(1<<PD6)|(1<<PD7);
	PORTB|=(1<<PB0);
	
	// if 1-4 is given, ground it (send 0 to clear)
	if (reg==1) {PORTD&=~(1<<PD5);}
	if (reg==2) {PORTD&=~(1<<PD6);}
	if (reg==3) {PORTD&=~(1<<PD7);}
	if (reg==4) {PORTB&=~(1<<PB0);}
	
	_delay_ms(1);
	
	int val=0;
	if (PINC&(1<<PC5)){val+=1;}
	if (PINC&(1<<PC4)){val+=2;}
	if (PINC&(1<<PC3)){val+=4;}
	if (PINC&(1<<PC2)){val+=8;}
	if (PINC&(1<<PC1)){val+=16;}
	if (PINC&(1<<PC0)){val+=32;}
	if (PINB&(1<<PB2)){val+=64;}
	if (PINB&(1<<PB1)){val+=128;}
	
	
	// set all registers high
	PORTD|=(1<<PD5)|(1<<PD6)|(1<<PD7);
	PORTB|=(1<<PB0);
	
	return val;
}

unsigned long int counter_getCount(){
	unsigned long int count=0;
	count+=counter_reg(4);count<<=8;
	count+=counter_reg(3);count<<=8;
	count+=counter_reg(2);count<<=8;
	count+=counter_reg(1);
	return count;
}

unsigned long int counter_getCount_safe(){
	unsigned long int count1=1;
	unsigned long int count2=2;
	while (count1!=count2){
		count1=counter_getCount();
		count2=counter_getCount();
	}
	return count1;
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
	
	// register selects: PD5,PD6,PD7,PB0
	DDRD|=(1<<PD5)|(1<<PD6)|(1<<PD7);
	DDRB|=(1<<PB0);
	
	// LED output
	DDRD|=(1<<PD0); // LED1
	DDRD|=(1<<PD4); // LED2
	
	// count reads: PC5,PC4,PC3,PC2,PC1,PC0,PB2,PB1
	// INPUTS NOT OUTPUTS
	
	serial_init();
		
	serial_string("Frequency Counter");
	serial_break();
	serial_string("www.SWHarden.com");
	serial_break();
	
	unsigned long int countOld;
	unsigned long int countNew;
	unsigned long int countDiff;
	char blankReads=0;
	for(;;){		
		serial_number(counter_getCount_safe()); // send the difference
		serial_break();
		_delay_ms(100);
		//serial_send(0b10101010);
	}	
}