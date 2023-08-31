#define F_CPU 24000000UL
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "audio.h"


/* Controls output level of pin 32 */
void setup_TCB_PWM(){
	// Enable this peripheral
	TCB0.CTRLA |= TCB_ENABLE_bm;
	
	// Make waveform output available on the pin
	TCB0.CTRLB |= TCB_CCMPEN_bm;
	
	// Enable 8-bit PWM mode
	TCB0.CTRLB |= TCB_CNTMODE_PWM8_gc;
	
	// Set period and duty
	TCB0.CCMPL = 255; // top value
	TCB0.CCMPH = 50; // flip value
}

/* Controls when waveform advances */
void setup_TCA_Advance(){
	// enable this peripheral
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;

	// Overflow triggers interrupt
	TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;

	// Set period and duty
	const int SAMPLE_RATE = 8000;
	TCA0.SINGLE.PER = F_CPU/SAMPLE_RATE;
}

volatile long AUDIO_INDEX = 0;

ISR(TCA0_OVF_vect)
{
	uint8_t data;
	if (AUDIO_INDEX < sizeof(AUDIO_SAMPLES_1)){
		data = pgm_read_byte(&AUDIO_SAMPLES_1[AUDIO_INDEX]);
		} else {
		long index = AUDIO_INDEX-sizeof(AUDIO_SAMPLES_1);
		data = pgm_read_byte(&AUDIO_SAMPLES_2[index]);
	}
	DAC0.DATA = data << 5;
	
	AUDIO_INDEX++;
	
	const long AUDIO_SAMPLE_COUNT = sizeof(AUDIO_SAMPLES_1) + sizeof(AUDIO_SAMPLES_2);
	if (AUDIO_INDEX >= AUDIO_SAMPLE_COUNT){
		AUDIO_INDEX = 0;
	}
	
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; // indicate interrupt was handled
}

void setup_DAC(){
	DAC0.CTRLA = DAC_OUTEN_bm | DAC_ENABLE_bm;
}

int main(void)
{
	CCP = CCP_IOREG_gc; // Protected write
	CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_24M_gc; // Set clock to 24MHz
	
	PORTA.DIR = 0xFF;
	PORTD.DIR = 0xFF;

	//setup_TCB_PWM();
	setup_DAC();
	setup_TCA_Advance();
	sei();

	while (1)
	{
		_delay_ms(500);
		PORTD.OUT = 255;
		TCB0.CCMPH = 50;
		_delay_ms(500);
		PORTD.OUT = 0;
		TCB0.CCMPH = 200;
	}
}

