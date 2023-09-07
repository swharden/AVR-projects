#define F_CPU 24000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*

OUTPUT
PD1 (pin 11) - A
PD2 (pin 12) - B
PD3 (pin 13) - C
PD4 (pin 14) - D
PD5 (pin 15) - E
PD6 (pin 16) - F
PD7 (pin 17) - G
PF2 (pin 22) - point

INPUT
PC0 (pin 6)
PC1 (pin 7)
PC2 (pin 8)
PC3 (pin 9)

*/

void display_setup(){
	PORTD.DIR = 255;
	PORTF.DIR = 255;
	PORTD.OUT = 255;
	PORTF.OUT = 255;
	
	PORTC.DIRSET = 255;
	PORTC.OUTCLR = 255;
}

void display_digit(uint8_t digit, uint8_t number){
	
}

int main(void)
{
	CCP = CCP_IOREG_gc; // Protected write
	CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_24M_gc; // Set clock to 24MHz
	display_setup();
	//display(0, );
}