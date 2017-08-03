#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

// LED ON AND OFF FUNCTIONS
void LED_POWER_ON(){PORTD|=(1<<PD2);}
void LED_POWER_OFF(){PORTD&=~(1<<PD2);}
void LED_POWER_TOGGLE(){PORTD^=(1<<PD2);}
void LED_PV_ON(){PORTD|=(1<<PD3);}
void LED_PV_OFF(){PORTD&=~(1<<PD3);}
void LED_CF_ON(){PORTD|=(1<<PD4);}
void LED_CF_OFF(){PORTD&=~(1<<PD4);}
void LED_EN_ON(){PORTD|=(1<<PD5);}
void LED_EN_OFF(){PORTD&=~(1<<PD5);}
void LED_LASER_IN_ON(){PORTB|=(1<<PB0);}
void LED_LASER_IN_OFF(){PORTB&=~(1<<PB0);}
void LED_LASER_OUT_ON(){PORTB|=(1<<PB1);}
void LED_LASER_OUT_OFF(){PORTB&=~(1<<PB1);}

// SERVO CONTROL
// Model SG90: http://www.micropik.com/PDF/SG90Servo.pdf
// note that the servo line gets inverted
void SERVO_LOW(){PORTB|=(1<<PB2);}
void SERVO_HIGH(){PORTB&=~(1<<PB2);}

void wait(){_delay_ms(30);}
void test(){
		LED_POWER_ON();wait();
		LED_PV_ON();wait();
		LED_CF_ON();wait();
		LED_EN_ON();wait();
		LED_LASER_IN_ON();wait();
		LED_LASER_OUT_ON();wait();
		LED_POWER_OFF();wait();
		LED_PV_OFF();wait();
		LED_CF_OFF();wait();
		LED_EN_OFF();wait();
		LED_LASER_IN_OFF();wait();
		LED_LASER_OUT_OFF();wait();
}

void SHUTTER_OPEN(){
	SET_POSITION(12);
	LED_LASER_OUT_ON();
}

void SHUTTER_CLOSE(){
	SET_POSITION(18);
	LED_LASER_OUT_OFF();
}

volatile int POSITION;
void SET_POSITION(int position){
	
	// is this a new position?
	if (POSITION==position){
		return;
	} else {
		POSITION=position;
	}
	
	// position can be 10-20
	// big swings seem to be faster than small ones
	int repeats=15;
	while (repeats>0){
		LED_POWER_TOGGLE();
		int ticks = 0; // 10 ticks per ms
		SERVO_HIGH();
		while (ticks<position){
			_delay_us(96);
			ticks+=1;
		}
		SERVO_LOW();
		while (ticks<200){
			_delay_us(96);
			ticks+=1;
		}
		repeats-=1;
	}
}

void poll(){
	LED_POWER_ON();
	
	// SENSE INPUTS
	char input_PV = 0;
	char input_CF = 0;
	if (!(PINB&(1<<PB4))) input_PV=1;
	if (!(PINB&(1<<PB3))) input_CF=1;
	
	// SENSE BUTTON STATES	
	char button_PV = 0;	
	char button_CF = 0;	
	char button_EN = 0;	
	if (!(PINA&(1<<PA0))) button_PV=1;
	if (!(PINA&(1<<PA1))) button_CF=1;
	if (!(PIND&(1<<PD1))) button_EN=1;
	
	// UPDATE DISPLAY (simple)
	if (input_PV) {LED_PV_ON();} else {LED_PV_OFF();}
	if (input_CF) {LED_CF_ON();} else {LED_CF_OFF();}
	if (button_EN) {LED_EN_ON();} else {LED_EN_OFF();}
	
	// GATE LASER INPUT
	if ((input_PV && button_PV)||(input_CF && button_CF)){
		LED_LASER_IN_ON();
		if (button_EN){SHUTTER_OPEN();} else {SHUTTER_CLOSE();}
	} else {
		LED_LASER_IN_OFF();
		SHUTTER_CLOSE();
	}

}

int main(void){
	// all pins are inputs unless otherwise defined here
	DDRA=0;
	DDRB=(1<<PB0)|(1<<PB1); // LEDs
	DDRB|=(1<<PB2); // servo
	DDRD=(1<<PD2)|(1<<PD3)|(1<<PD4); // LEDs
	PORTA=(1<<PA0)|(1<<PA1); // pull buttons high
	PORTD=(1<<PD1); // pull buttons high

	// always test the LEDs when we start
	char i; for (i=0;i<3;i++){test();}

	// if all buttons are unpressed, enter a test mode
	while ((PINA&(1<<PA0))&&(PINA&(1<<PA1))&&(PIND&(1<<PD1))){
		SHUTTER_OPEN();
		test();test();
		SHUTTER_CLOSE();
		test();test();
	} 
	
	// the main program continuously polls the inputs
	for(;;){poll();}
}