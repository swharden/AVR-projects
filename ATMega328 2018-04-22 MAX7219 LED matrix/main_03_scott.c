/*

displays data from a matrix

*/

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>

#define nCols 32 // LED matrix width (8 columns per segment)
#define nRows 8 // LED matrix height (8 rows per segment)

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
	uint8_t i = nCols/8;
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
	LEDsendInit(0x0A,0x0f); // intensity: brightest (last byte)
	LEDsendInit(0x0B,0x07); // scan limit (number of rows in matrix)
}







///////// MATRIX THINGS //////////////////


volatile uint8_t data[nCols];

void MatrixClear(){
	uint8_t i;
	for (i=0;i<sizeof(data);i++){
		data[i]=0;
	}
}

void MatrixRandom(){
	uint8_t i;
	for (i=0;i<sizeof(data);i++){
		data[i]=rand();
	}
}

void MatrixRollRight(){
	uint8_t i;
	uint8_t lastByte=data[sizeof(data)-1];
	for (i=sizeof(data)-1;i>0;i--){
		data[i]=data[i-1];
	}
	data[0]=lastByte;
}

void MatrixRollLeft(){
	uint8_t i;
	uint8_t firstByte=data[0];
	for (i=0;i<sizeof(data)-1;i++){
		data[i]=data[i+1];
	}
	data[sizeof(data)-1]=firstByte;
}

void MatrixRollDown(){
	uint8_t byte;
	uint8_t i;
	for (i=0;i<sizeof(data);i++){
		byte=data[i];
		data[i] = byte<<1;
		data[i] += (byte>>7)&1;
	}
}

void MatrixPointOn(uint8_t x, uint8_t y){
	uint8_t byte=data[x];
	byte|=(1<<y);
	data[x]=byte;
}

uint8_t MatrixDisplayByte(uint8_t segment, uint8_t row){
	uint8_t val=0;
	uint8_t i;
	uint8_t thisBit;
	for (i=0; i<8; i++) {
		thisBit = ((data[segment*8+i])>>row)&1;
		val += thisBit<<(7-i);
	}
	return val;
}

void MatrixDisplay(){
	uint8_t row;
	uint8_t seg;
	for (row=0;row<8;row++){
		PORTB &=~(1<<PB2); // set SS low
		for (seg=0;seg<4;seg++){
			spi_tranceiver(row+1);
			spi_tranceiver(MatrixDisplayByte(seg,row));
		}
		PORTB |= (1<<PB2); // set SS high
	}	
}

int main(void){

	spi_init_master();	
	LEDmatrixInit();

	DDRB |= (1<<PB1); // set PD6 (pin 11) output (LED)

	MatrixClear();
	//MatrixRandom();

	// S
	MatrixPointOn(1,1);
	MatrixPointOn(2,1);
	MatrixPointOn(3,1);
	MatrixPointOn(1,2);
	MatrixPointOn(1,3);
	MatrixPointOn(1,3);
	MatrixPointOn(2,3);
	MatrixPointOn(3,3);
	MatrixPointOn(3,4);
	MatrixPointOn(3,5);
	MatrixPointOn(2,5);
	MatrixPointOn(1,5);

	// C
	MatrixPointOn(6,3);
	MatrixPointOn(5,3);
	MatrixPointOn(5,4);
	MatrixPointOn(5,5);
	MatrixPointOn(6,5);

	// O
	MatrixPointOn(9,3);
	MatrixPointOn(8,3);
	MatrixPointOn(8,4);
	MatrixPointOn(8,5);
	MatrixPointOn(9,5);
	MatrixPointOn(10,5);
	MatrixPointOn(10,4);
	MatrixPointOn(10,3);

	// T
	MatrixPointOn(12,5);
	MatrixPointOn(12,4);
	MatrixPointOn(12,3);
	MatrixPointOn(12,2);
	MatrixPointOn(12,1);
	MatrixPointOn(11,1);
	MatrixPointOn(13,1);

	// T
	MatrixPointOn(16,5);
	MatrixPointOn(16,4);
	MatrixPointOn(16,3);
	MatrixPointOn(16,2);
	MatrixPointOn(16,1);
	MatrixPointOn(15,1);
	MatrixPointOn(17,1);

	// smile
	
	MatrixPointOn(23,1);
	MatrixPointOn(23,2);
	MatrixPointOn(23,3);
	
	MatrixPointOn(25,1);
	MatrixPointOn(25,2);
	MatrixPointOn(25,3);

	MatrixPointOn(24,5);
	MatrixPointOn(23,5);
	MatrixPointOn(25,5);
	MatrixPointOn(22,5);
	MatrixPointOn(26,5);
	MatrixPointOn(21,4);
	MatrixPointOn(27,4);

	uint8_t i=0;
	for(;;){
		i += 1;
		MatrixDisplay();
		_delay_ms(20);
		MatrixRollLeft();
		if (i%3==0) MatrixRollDown();
	}
}
