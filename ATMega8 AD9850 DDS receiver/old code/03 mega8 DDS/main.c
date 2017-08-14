#define F_CPU 20000000UL

#include <avr/io.h>
#include <util/delay.h>


#define bigMult 4294967296
#define crystalFreq 125000000
#define ddsDDR DDRD
#define ddsPORT PORTD
#define ddsPinClock 2
#define ddsPinLoad 3
#define ddsPinData 4


void tick(){
	_delay_ms(1);
}

void ckPulse(){
	// pulse the clock one cycle
	ddsPORT|=(1<<ddsPinClock);
	tick();
	ddsPORT&=~(1<<ddsPinClock);
}

void loadPulse(){
	// raise the load pin two ticks
	ddsPORT|=(1<<ddsPinLoad);
	tick();	tick();
	ddsPORT&=~(1<<ddsPinLoad);
}

void freqSet(uint64_t freq){
	// set the frequency to this value in Hz
	uint64_t bigNum = freq << 32; // multiply by 2^32
	bigNum = bigNum/crystalFreq;
}

void sendBit(char toSend){
	if (toSend==1) {ddsPORT|=(1<<ddsPinData);}
	ckPulse();
	ddsPORT&=~(1<<ddsPinData);
}

void goToCode(unsigned long int freqCode){
	char i=0;
	ckPulse();tick();loadPulse();tick(); // initiate instruction
	for (i=0;i<32;i++){sendBit((freqCode>>i)&1);} // send frequency code
	for (i=0;i<8;i++){sendBit(0);} // send control code (eight zeros)
	loadPulse(); // finalize instructions
}


unsigned long int freq2code(unsigned long freqHz){
	unsigned long int freqCode;
	freqCode = freqHz*bigMult; //4294967296;
	freqCode = freqCode/crystalFreq;
	return (unsigned long int) freqCode;
}

void splash(){
	char i;
    for(i=0;i<20;i++) {
		PORTD=255;
		_delay_ms(50);
		PORTD=0;
		_delay_ms(50);
	}
}

int main (void)
{
    DDRD = 255; 
	//splash();
	//goToCode(freq2code(10140000));
	_delay_ms(1000);
	for(;;){
		//goToCode(freq2code(10000000));
		goToCode(240518169);
		//PORTD^=(1<<PD7);
		_delay_ms(1000);
	}
}