#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>




#define bigMult 4294967296
#define crystalFreq 125000000
#define ddsDDR DDRD
#define ddsPORT PORTD
#define ddsPinClock 2
#define ddsPinLoad 3
#define ddsPinData 4

void ckPulse(){
	// pulse the clock one cycle
	ddsPORT|=(1<<ddsPinClock);
	ddsPORT&=~(1<<ddsPinClock);
}

void loadPulse(){
	ddsPORT|=(1<<ddsPinLoad);
	ddsPORT&=~(1<<ddsPinLoad);
}

void freqSet(uint64_t freq){
	// set the frequency to this value in Hz
	uint64_t bigNum = freq << 32; // multiply by 2^32
	bigNum = bigNum/crystalFreq;
	char i;
}

void sendBit(char toSend){
	if (toSend==1) {ddsPORT|=(1<<ddsPinData);}
	ckPulse();
	ddsPORT&=~(1<<ddsPinData);
}

void goToCode(unsigned long int freqCode){
	char i=0;
	ckPulse();loadPulse(); // initiate instruction
	for (i=0;i<32;i++){sendBit((freqCode>>i)&1);} // send frequency code
	for (i=0;i<8;i++){sendBit(0);} // send control code (eight zeros)
	loadPulse(); // finalize instructions
}


unsigned long int freq2code(unsigned long int freqHz){
	unsigned long long int freqCode;
	freqCode = freqHz*bigMult; //4294967296;
	freqCode = freqCode/crystalFreq;
	return (unsigned long int) freqCode;
}






int main(void){
	DDRD=255;
	long int baseFreq;
	char i=0;
	
	for(;;){
	
		
		baseFreq=10140000-664;
		while (i<10){
			i++;
			goToCode(freq2code(baseFreq+i));
			_delay_ms(3000);
		}
		while (i>0){
			i--;
			goToCode(freq2code(baseFreq+i));
			_delay_ms(10000);
		}
	
		baseFreq=7000368;
		while (i<10){
			i++;
			goToCode(freq2code(baseFreq+i));
			_delay_ms(3000);
		}
		while (i>0){
			i--;
			goToCode(freq2code(baseFreq+i));
			_delay_ms(10000);
		}
		
	}
}
