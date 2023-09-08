/*
This project uses an AVR64DD32 to directly drive segments of a multiplexed 
4-digit 7-segment display to display ADC values read by a HX710 ADC.

DISPLAY: SOURCE CURRENT
PD1 (pin 11) - A
PD2 (pin 12) - B
PD3 (pin 13) - C
PD4 (pin 14) - D
PD5 (pin 15) - E
PD6 (pin 16) - F
PD7 (pin 17) - G
PF2 (pin 22) - point

DISPLAY: SINK CURRENT
PC0 (pin 6)
PC1 (pin 7)
PC2 (pin 8)
PC3 (pin 9)

HX710 SPI ADC:
OUT PA5 (pin 3)
CLK PA6 (pin 4)
*/

#define F_CPU 24000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void setup_display(){
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
		PORTF.OUTCLR = PIN2_bm;
		next_digit = 1;
		} else if (next_digit == 1){
		PORTC.OUT = 0b00001101;
		PORTD.OUT = digit_masks[digit_2];
		PORTF.OUTSET = PIN2_bm;
		next_digit = 2;
		} else if (next_digit == 2){
		PORTC.OUT = 0b00001011;
		PORTD.OUT = digit_masks[digit_3];
		PORTF.OUTCLR = PIN2_bm;
		next_digit = 3;
		} else if (next_digit == 3){
		PORTC.OUT = 0b00000111;
		PORTD.OUT = digit_masks[digit_4];
		PORTF.OUTCLR = PIN2_bm;
		next_digit = 0;
	}
	
	TCB0.INTFLAGS = TCB_OVF_bm | TCB_CAPT_bm;
}

#define SPI_CS_LOW() PORTA.OUTCLR = PIN7_bm
#define SPI_CS_HIGH() PORTA.OUTSET = PIN7_bm

uint8_t SPI_SEND(uint8_t data){
	SPI0.DATA = data;
	while (!(SPI0.INTFLAGS & SPI_IF_bm));
	return SPI0.DATA;
}

#define HX_MODE_DIFF_10HZ 1
#define HX_MODE_TEMP_40HZ 2
#define HX_MODE_DIFF_40HZ 3

unsigned long readHX() {

	// wait for the reading to finish
	while (PORTA.IN & PIN5_bm) {}

	// read the 24-bit pressure
	unsigned long result = 0;
	for (int i=0; i<24; i++){
		PORTA.OUTSET = PIN6_bm; // SCK
		PORTA.OUTCLR = PIN6_bm; // SCK
		
		result = result << 1;
		if((PORTA.IN & PIN5_bm)) {
			result++;
		}
	}
	
	// pulse clock line to start a reading
	for (char i = 0; i < HX_MODE_DIFF_40HZ; i++) {
		PORTA.OUTSET = PIN6_bm; // SCK
		PORTA.OUTCLR = PIN6_bm; // SCK
	}
	
	result=result ^ 0x800000;	return result;
}

int main(void)
{
	CCP = CCP_IOREG_gc; // Protected write
	CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_24M_gc; // Set clock to 24MHz
	setup_display();
	setup_timer();
	
	PORTA.DIRCLR = PIN5_bm; // MISO
	PORTA.OUTCLR = PIN5_bm;
	PORTA.DIRSET = PIN6_bm; // SCK
	
	sei();
	
	for(;;){
		unsigned long pressure_raw = readHX();
		uint16_t digits = pressure_raw / 10000;
		set_digits(digits);
	}
}