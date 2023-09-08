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
	PORTF.DIRSET = PIN2_bm;
	PORTC.DIRSET = 0b00001111;
}

uint8_t digit_masks[] = {
	0b01111110,
	0b00001100,
	0b10110110,
	0b10011110,
	0b11001100,
	0b11011010,
	0b11111010,
	0b00001110,
	0b11111110,
	0b11011110,
	0b10000000,
};

void setup_timer(){
	TCB0.INTCTRL |= TCB_CAPT_bm;
	TCB0.CCMP = 48000; // 500 Hz @ 24 MHz clock
	TCB0.CTRLA |= TCB_ENABLE_bm;
}

volatile uint8_t next_digit;
volatile uint8_t digit_1;
volatile uint8_t digit_2;
volatile uint8_t digit_3;
volatile uint8_t digit_4;

void set_digits(uint16_t number){
	digit_4 = number % 10;
	number /= 10;
	digit_3 = number % 10;
	number /= 10;
	digit_2 = number % 10;
	number /= 10;
	digit_1 = number % 10;
}

ISR(TCB0_INT_vect)
{
	if (next_digit == 0){
		PORTC.OUT = 0b00001110;
		PORTD.OUT = digit_masks[digit_1];
		next_digit = 1;
		} else if (next_digit == 1){
		PORTC.OUT = 0b00001101;
		PORTD.OUT = digit_masks[digit_2];
		PORTF.OUTSET = PIN2_bm;
		_delay_ms(1);
		PORTF.OUTCLR = PIN2_bm;
		next_digit = 2;
		} else if (next_digit == 2){
		PORTC.OUT = 0b00001011;
		PORTD.OUT = digit_masks[digit_3];
		next_digit = 3;
		} else if (next_digit == 3){
		PORTC.OUT = 0b00000111;
		PORTD.OUT = digit_masks[digit_4];
		next_digit = 0;
	}
	
	TCB0.INTFLAGS = TCB_OVF_bm | TCB_CAPT_bm;
}

int main(void)
{
	CCP = CCP_IOREG_gc; // Protected write
	CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_24M_gc; // Set clock to 24MHz
	display_setup();
	
	setup_timer();
	sei();
	
	uint16_t count = 0;
	for(;;){
		set_digits(count++);
		_delay_ms(100);
	}
}