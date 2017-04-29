#define F_CPU 1000000ul
#include <avr/io.h>
#include <util/delay.h>
#include "serial_m328.c"
#include "i2c_master.c"

int readTemp(){
	// read LM75A sensor and return temperature as raw value
	// divide this value by 8 and you have temperature in C
	uint8_t data[2];
	uint8_t address=0b10010001; // 0b1001xxx1 where x is voltage on A2, A1, A0
	int temperature;
	i2c_receive(address,data,2);
	temperature=(data[0]*256+data[1])/32;
	return temperature;
}

int sendTemp(){
	// read temperature and send it as ASCII
	int temp;
	temp=readTemp();
	serial_number3(temp/8);
	temp-=8*(temp/8);
	temp*=125;
	serial_send('.');
	serial_number3(temp);
	serial_break();
}

int main(void){
	_delay_ms(200);
	serial_init();
	i2c_init();
	for(;;){
		_delay_ms(200);
		sendTemp();
		PORTC^=(1<<PC3); // blink LED
	}
}