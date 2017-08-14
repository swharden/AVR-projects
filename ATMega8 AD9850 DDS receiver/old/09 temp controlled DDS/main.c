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

#define INVERTED 0

volatile long int baseFreq[] = 
{
	10140000+66,	// 30m target 10.140.000 MHz
	 7000000+890,	// 40m target  7.000.850 MHz
	14000000+830	// 20m target 14.000.750 MHz
};
volatile char band=0; 	// set to desired start-up band
volatile char offset=0; // shift this for FSK




void ckPulse(){
	if (INVERTED){
		ddsPORT&=~(1<<ddsPinClock);
		ddsPORT|=(1<<ddsPinClock);
	} else {
		ddsPORT|=(1<<ddsPinClock);
		ddsPORT&=~(1<<ddsPinClock);
	}
}

void loadPulse(){
	if (INVERTED){
		ddsPORT&=~(1<<ddsPinLoad);
		ddsPORT|=(1<<ddsPinLoad);
	} else {
		ddsPORT|=(1<<ddsPinLoad);
		ddsPORT&=~(1<<ddsPinLoad);
	}
}

void freqSet(uint64_t freq){
	uint64_t bigNum = freq << 32; // multiply by 2^32
	bigNum = bigNum/crystalFreq;
	char i;
}

void sendBit(char toSend){
	if (INVERTED){
		if (toSend==1) {ddsPORT&=~(1<<ddsPinData);}
		ckPulse();
		ddsPORT|=(1<<ddsPinData);
	} else {
		if (toSend==1) {ddsPORT|=(1<<ddsPinData);}
		ckPulse();
		ddsPORT&=~(1<<ddsPinData);
	}
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


void DDS_setup(){
	ddsDDR |= (1<<ddsPinClock) | (1<<ddsPinLoad) | (1<<ddsPinData);
	if (INVERTED) {
		ddsPORT |= (1<<ddsPinClock) | (1<<ddsPinLoad) | (1<<ddsPinData);
	}
}

void DDS_updateFreq(){
	if (offset>99){
		goToCode(0);
	}else{
		goToCode(freq2code(baseFreq[band]+offset));
	}
}



void pushedENTER() {
	band+=1;
	if (band>=3) {band=0;}
	DDS_updateFreq();
} 

void pushedUP() {
	baseFreq[band]+=1;
	DDS_updateFreq();
}

void pushedDOWN() {
	baseFreq[band]-=1;
	DDS_updateFreq();
}

void ledON(){PORTD|=(1<<PD7);}
void ledOFF(){PORTD&=~(1<<PD7);}

void debounce(){
	ledON();
	loop_until_bit_is_set(PINB, PB0);
	loop_until_bit_is_set(PINB, PB1);
	loop_until_bit_is_set(PINB, PB2);
	_delay_ms(10);
	ledOFF();
}


void hold(int sec10){
	while (sec10){
		if (bit_is_clear(PINB,PB0)) {debounce();pushedENTER();_delay_ms(100);}
		if (bit_is_clear(PINB,PB1)) {debounce();pushedDOWN();_delay_ms(100);}
		if (bit_is_clear(PINB,PB2)) {debounce();pushedUP();_delay_ms(100);}
		sec10--;
		PORTD|=(1<<PD7);
		_delay_ms(1);
		PORTD&=~(1<<PD7);
		_delay_ms(99);
	}
}

#define FSK_HZ 5
#define FSK_TM 100

// asume prepended with a LOW
volatile int seconds=0;
/*
void de(){
	offset=FSK_HZ;
	DDS_updateFreq();	
	hold(FSK_TM);	
	offset=0;
	DDS_updateFreq();	
	hold(FSK_TM);	
	seconds+=FSK_TM*2;
}

void da(){
	offset=FSK_HZ;
	DDS_updateFreq();	
	hold(FSK_TM*3);	
	offset=0;
	DDS_updateFreq();	
	hold(FSK_TM);	
	seconds+=FSK_TM*5;
}
void sp(){	
	hold(FSK_TM*2);
	seconds+=FSK_TM*2;
}
*/

void de(){
	offset=0;
	DDS_updateFreq();
	hold(FSK_TM);	
	offset=100;
	DDS_updateFreq();
	hold(FSK_TM);	
	seconds+=FSK_TM+FSK_TM;
}

void da(){
	offset=FSK_HZ;
	DDS_updateFreq();
	hold(FSK_TM);	
	offset=100;
	DDS_updateFreq();
	hold(FSK_TM);
	seconds+=FSK_TM+FSK_TM;
}

void sp(){
	offset=100;
	DDS_updateFreq();
	hold(FSK_TM);	
	seconds+=FSK_TM;
}

void slant(){
	char i=0;
	for (i=0;i<10;i++){
		for (offset=0;offset<=5;offset++){
			DDS_updateFreq();
			hold(10);
			seconds+=1;
		}
		for (offset=5;offset>=0;offset--){
			DDS_updateFreq();
			hold(10);
			seconds+=1;
		}
	}
}

int main(void){
	DDS_setup();
	DDRD|=(1<<PD7); // LED
	DDRD=255;
	DDRB=0; // set all portB as input
	PORTB=255; // set inputs as HIGH
	int i=0;
	
	/*
	for (i=0;i<3;i++){
		band=i;
		DDS_updateFreq();
		hold(50);
	}
	*/
	
	band=1;
	DDS_updateFreq();
	
	/*
	for (i=0;i<600;i++){
		hold(100);
	}
	*/
	/*
	for(;;){
		de();
		da();
		
		sp();
		
		de();sp();
		da();sp();
	}
	*/
	
	for(;;){
	
		offset=FSK_HZ;
		DDS_updateFreq();
		_delay_ms(1000);
		//hold(FSK_TM);	
		PORTD^=(1<<PD7);
	
		/*
		slant();sp();
		de();da();sp();
		de();da();da();da();sp();
		de();de();de();de();da();sp();
		de();de();de();da();sp();
		da();de();de();sp();
		sp();sp();
		
		while (seconds<60*10){
			hold(10);
			seconds+=1;
		}
		seconds=0;*/
	}
}
