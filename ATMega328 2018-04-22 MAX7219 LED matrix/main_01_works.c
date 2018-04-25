/*

demonstrates how to illuminate a single point on the LED matrix

https://github.com/dhepper/font8x8/blob/master/font8x8_basic.h
https://www.sparkfun.com/datasheets/Components/General/COM-09622-MAX7219-MAX7221.pdf

*/
#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>

uint8_t nSegments=4;
uint8_t nCols=8*4;
uint8_t nRows=8;

void spi_init_master (void)
{
    DDRB |= (1<<PB5); // set SCK as output
	DDRB |= (1<<PB3); // set MOSI as output
	DDRB |= (1<<PB2); // set SS as output
	PORTB |= (1<<PB2); // set SS high

	SPCR |= (1<<SPE); // enable SPI
	SPCR |= (1<<MSTR); // this device is SPI master
	SPCR |= (1<<SPR0); // clock prescale osc/16
}

void spi_tranceiver(unsigned char data)
{
    SPDR = data; // Load data into the buffer
    while(!(SPSR&(1<<SPIF))); //Wait until transmission complete
}

void LEDsendInit(uint8_t address, uint8_t data){
	uint8_t i = nSegments;
	PORTB &=~(1<<PB2); // set SS low
	while (i-->0){
		spi_tranceiver(address);
		spi_tranceiver(data);
	}
	PORTB |= (1<<PB2); // set SS high
}

void LEDsend(uint8_t address, uint8_t data){
	PORTB &=~(1<<PB2); // set SS low
	spi_tranceiver(address);
	spi_tranceiver(data);
	PORTB |= (1<<PB2); // set SS high
}

void LEDmatrixInit(){
	LEDsendInit(0x0C,0x01); // shutdown mode: normal operation
	LEDsendInit(0x09,0x00); // decode mode: no character decoding
	LEDsendInit(0x0A,0x0F); // intensity: brightest (last byte)
	LEDsendInit(0x0B,0x07); // scan limit (number of rows in matrix)
}

void LEDtest2(uint8_t lightCol, uint8_t lightRow){
	// send a column and row too big to be real to clear the LED

	uint8_t row;
	uint8_t seg;

	// create an empty matrix and fill it with zeros
	uint8_t matrix[4][8];
	for (row=0;row<8;row++){
		for (seg=0;seg<4;seg++){
			matrix[seg][row]=0;
		}
	}

	// determine which bit to flip in the matrix
	lightCol=(8*4)-lightCol;
	uint8_t lightSeg=lightCol/8;
	uint8_t lightColInSeg=lightCol-(lightSeg*8);
	uint8_t lightData=(1<<(8-lightColInSeg));
	if (lightColInSeg==0) {
		lightData=0b00000001;
		lightSeg-=1;
	}
	matrix[lightSeg][lightRow]=lightData;

	// use the matrix to send commands to the display
	for (row=0;row<8;row++){
		PORTB &=~(1<<PB2); // set SS low
		for (seg=0;seg<4;seg++){
			spi_tranceiver(row+1);
			spi_tranceiver(matrix[seg][row]);
		}
		PORTB |= (1<<PB2); // set SS high
	}	
}

int main(void){

	spi_init_master();	
	LEDmatrixInit();

	DDRB |= (1<<PB1); // set PD6 (pin 11) output (LED)

	uint8_t lightCol=0;
	uint8_t lightRow=0;
	for(;;){		
		lightCol++;
		if (lightCol>=8*4) {
			lightCol=0;
			lightRow++;
			if (lightRow>=8) lightRow=0;
		}
		LEDtest2(lightCol,lightRow);
		_delay_ms(10); // wait
	}
}
