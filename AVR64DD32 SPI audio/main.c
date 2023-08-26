#define F_CPU 24000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SPI_CS_LOW() PORTA.OUTCLR = PIN7_bm
#define SPI_CS_HIGH() PORTA.OUTSET = PIN7_bm

uint8_t SPI_SEND(uint8_t data){
	SPI0.DATA = data;
	while (!(SPI0.INTFLAGS & SPI_IF_bm));
	return SPI0.DATA;
}

void SPI_read_id(){
	SPI_CS_LOW();
	SPI_SEND(0x90);
	SPI_SEND(0);
	SPI_SEND(0);
	SPI_SEND(0);
	SPI_SEND(0);
	SPI_SEND(0);
	SPI_CS_HIGH();
}

uint8_t SPI_read_byte(long address){
	uint8_t pageA = address >> 16;
	uint8_t pageB = address >> 8;
	uint8_t pageC = address >> 0;

	SPI_CS_LOW();
	SPI_SEND(0x03);
	SPI_SEND(pageA);
	SPI_SEND(pageB);
	SPI_SEND(pageC);
	uint8_t result = SPI_SEND(0xFF);
	SPI_CS_HIGH();
	
	return result;
}

/* Controls output level of pin 32 */
void setup_TCB_PWM(){
	// Set pin as output
	PORTA.DIR = 0xFF;
	PORTD.DIR = 0xFF;
	
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

volatile long AUDIO_ADDRESS;

ISR(TCA0_OVF_vect)
{
	uint8_t val = SPI_read_byte(AUDIO_ADDRESS);
	// wait until next rollover to update
	while(TCB0.CNT > 0){}
	TCB0.CCMPH = val;
	AUDIO_ADDRESS++;
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; // indicate interrupt was handled
}

int main(void)
{
	// set the clock
	CCP = CCP_IOREG_gc; // Protected write
	CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_24M_gc; // Set clock to 24MHz
	
	// setup LED
	PORTD.DIRSET = PIN7_bm;
	
	// setup SPI
	PORTA.PIN7CTRL |= PORT_PULLUPEN_bm; // CS
	PORTA.OUTSET = PIN7_bm; // CS
	PORTA.DIRSET = PIN4_bm; // MOSI
	PORTA.DIRCLR = PIN5_bm; // MISO
	PORTA.DIRSET = PIN6_bm; // SCK
	PORTA.DIRSET = PIN7_bm; // CS
	SPI0.CTRLA = SPI_ENABLE_bm | SPI_MASTER_bm;

	setup_TCB_PWM();
	setup_TCA_Advance();
	sei();

	while (1)
	{
		PORTD.OUTSET = PIN7_bm;
		_delay_ms(50);
		PORTD.OUTCLR = PIN7_bm;
		_delay_ms(950);
	}
}