#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "i2c2/i2c2_master.h"
#include "lcdpcf8574/lcdpcf8574.h"


void sendNum(unsigned long int val){
	// send a number as ASCII text
	unsigned long int divby=1000000000;
	while (divby>=1){
		lcd_putc('0'+val/divby);
		val-=(val/divby)*divby;
		divby/=10;
		if (divby==(100)){lcd_putc(',');}
		if (divby==(100000)){lcd_putc(',');}
		if (divby==(100000000)){lcd_putc(',');}
	}
}

int read(){
	// read and display pressure
	
	// python:
	//   BP.command("[0xEE 72]",silent=True) # set register
	//   BP.command("[0xEE 0]",silent=True) # ADC read
	//   return BP.i2c_read(3,'0xef',silent=True)
	
	uint8_t data[6];
	unsigned long int pressure=0;
	unsigned long int temperature=0;
	
	// GET PRESSURE
	data[0]=72; // command 72 is "set register to pressure"
	i2c2_transmit(0xEE,data,1); 
	_delay_ms(10); // give it time to make a read
	data[0]=0; // command 0 is "ADC read"
	i2c2_transmit(0xEE,data,1); 
	i2c2_receive(0xEF,data,3);
	pressure+=data[0];
	pressure=pressure<<8;
	pressure+=data[1];
	pressure=pressure<<8;
	pressure+=data[2];
	
	// GET TEMPERATURE
	data[0]=88; // command 88 is "set register to temperature"
	i2c2_transmit(0xEE,data,1); 
	_delay_ms(10); // give it time to make a read
	data[0]=0; // command 0 is "ADC read"
	i2c2_transmit(0xEE,data,1); 
	i2c2_receive(0xEF,data,3);
	temperature+=data[0];
	temperature=temperature<<8;
	temperature+=data[1];
	temperature=temperature<<8;
	temperature+=data[2];
		
	// DISPLAY RAW DATA ON DISPLAY
	lcd_gotoxy(0, 0);
	lcd_puts("P=");
	sendNum(pressure);
	lcd_gotoxy(0, 1);
	lcd_puts("T=");
	sendNum(temperature);
	
}



int main(void)
{
    lcd_init(LCD_DISP_ON);
	lcd_led(1);
    lcd_home();
	
	int i=1234;
    while(1) {
		read();
		i++;
    }
}


