#define F_CPU 11059200ul

#include <avr/io.h>
#include <util/delay.h>

#define USART_BAUDRATE 9600
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




////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

// these variables will hold an instant capture of all input pins
volatile uint8_t regB;
volatile uint8_t regC;
volatile uint8_t regD;

// these values will hold the current/voltage value
volatile char current[4];
volatile char voltage[4];


void read_capture(){
    // load the volatile capture variables with tue current pin state
    regB=PINB;
    regC=PINC;
    regD=PIND;
}

char saved_letter(){
    // returns the letter (1-8) of the segments stored in read_capture
    // returned value will be in ASCII
    // returns ? if no letter is selected, or if it's confusing
    
    // start by creating a "letter" variable which defines pin states
    uint8_t letter=0;    
    if(regD&(1<<PD0)){letter|=(1<<7);} // letter 1 (voltage MSB)
    if(regD&(1<<PD2)){letter|=(1<<6);} // letter 2
    if(regD&(1<<PD3)){letter|=(1<<5);} // letter 3
    if(regD&(1<<PD4)){letter|=(1<<4);} // letter 4
    if(regD&(1<<PD5)){letter|=(1<<3);} // letter 5 (current MSB)
    if(regD&(1<<PD6)){letter|=(1<<2);} // letter 6
    if(regD&(1<<PD7)){letter|=(1<<1);} // letter 7
    if(regB&(1<<PB0)){letter|=(1<<0);} // letter 8

    // try to match expected pin states
    if (letter==0b10000000) {return '1';}
    if (letter==0b01000000) {return '2';}
    if (letter==0b00100000) {return '3';}
    if (letter==0b00010000) {return '4';}
    if (letter==0b00001000) {return '5';}
    if (letter==0b00000100) {return '6';}
    if (letter==0b00000010) {return '7';}
    if (letter==0b00000001) {return '8';}
    return '?';    
}

volatile uint8_t saved_period;
char saved_number(){
    // returns the numerical value (0-9) stored in read_capture
    // returned value will be in ASCII
    // returns 0 if no letter is selected, or if it's confusing
    int segments=0;
    if(regC&(1<<PC5)){segments|=(1<<7);} // A (MSB)
    if(regC&(1<<PC4)){segments|=(1<<6);} // B
    if(regC&(1<<PC3)){segments|=(1<<5);} // C
    if(regC&(1<<PC2)){segments|=(1<<4);} // D
    if(regC&(1<<PC1)){segments|=(1<<3);} // E
    if(regC&(1<<PC0)){segments|=(1<<2);} // F
    if(regB&(1<<PB2)){segments|=(1<<1);} // G (LSB)
    //if(regB&(1<<PB1)){}//H
    
    if(segments==0b00001000) {return '9';}
    if(segments==0b00000000) {return '8';}
    if(segments==0b00011110) {return '7';}
    if(segments==0b01000000) {return '6';}
    if(segments==0b01001000) {return '5';}
    if(segments==0b10011000) {return '4';}
    if(segments==0b00001100) {return '3';}
    if(segments==0b00100100) {return '2';}
    if(segments==0b10011110) {return '1';}
    if(segments==0b00000010) {return '0';}
    return '?';
}

uint8_t capture_letter(char letter){
    // loops until the current letter has been captured
    uint16_t tries=1000; // trial and error reveals this works
    while (tries){
        tries--;
        read_capture(); // take a new read
        if (saved_letter()==letter) {
            if (saved_number()!='?'){
                if (letter=='1'){voltage[0]=saved_number();}
                if (letter=='2'){voltage[1]=saved_number();}
                if (letter=='3'){voltage[2]=saved_number();}
                if (letter=='4'){voltage[3]=saved_number();}
                if (letter=='5'){current[0]=saved_number();}
                if (letter=='6'){current[1]=saved_number();}
                if (letter=='7'){current[2]=saved_number();}
                if (letter=='8'){current[3]=saved_number();}
                return 1;
            }
        }
    }
    return 0;
}

void capture_full(){
    // capture even value of every letter
    char i;
    char j;
    for(i=0;i<4;i++) {voltage[i]=' ';}
    for(i=0;i<4;i++) {current[i]=' ';}
    
    for(i=0;i<8;i++){ // for each of the 8 letters
        for (j=0;j<10;j++){ // try to capture each letter 10 times
            if (capture_letter('1'+i)) {break;}
        }
    }
    
    for(i=0;i<4;i++) {
        serial_send(voltage[i]);
        if (i==1){serial_send('.');}
    }
    serial_send(',');
    for(i=0;i<4;i++) {
        serial_send(current[i]);
        if (i==2){serial_send('.');}
    }
    serial_break();
}

volatile uint16_t checksum;
int main(void){
    serial_init();
	for(;;){
        //serial_send(getDigitForLetter('1'));
        capture_full();
        //capture_letter('4');
        //serial_break();
	}
}
