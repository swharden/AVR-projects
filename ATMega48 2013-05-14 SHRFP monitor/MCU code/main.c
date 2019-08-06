#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

void tick(char ticks){
	while (ticks>0){
		_delay_us(100);
		ticks--;
	}
}

void pulse(char ticks){
	PORTB=255;
	tick(ticks);
	PORTB=0;
	tick(ticks);
}

void send_sync(){
	char i;
	for (i=0;i<10;i++){
		pulse(1);
		pulse(2);
		pulse(3);
	}
	pulse(3);
}

void send_lose(){
	char i;
	for (i=0;i<5;i++){
		pulse(3);
	}
}

void sendByte(int val){
	// TODO - make faster by only sending needed bytes
	char i;
	for (i=0;i<8;i++){
		if ((val>>i)&1){pulse(2);}
		else{pulse(1);}
	}
}

void send(int val){
	sendByte(val);  // regular
	sendByte(~val); // inverted
	pulse(3);
}

int main (void)
{
    DDRB = 255;
	int i;

    while(1) {
		send_sync();
		for (i=0;i<200;i++){
			send(i);
		}
		send_lose();
	}
}