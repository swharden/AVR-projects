#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "i2c2/i2c2_master.h"
#include "lcdpcf8574/lcdpcf8574.h"

void sendInt2(unsigned int val, int padding){
	char nextLetter;
	unsigned int divby=10;
	while (divby>=1){
		nextLetter='0'+val/divby;
		if ((nextLetter=='0')&&(padding==0)&&divby>=10){
			nextLetter=' ';} else {padding=1;}
		lcd_putc(nextLetter);
		val-=(val/divby)*divby;
		divby/=10;
	}
}

void sendInt3(unsigned int val, int padding){
	char nextLetter;
	unsigned int divby=100;
	while (divby>=1){
		nextLetter='0'+val/divby;
		if ((nextLetter=='0')&&(padding==0)&&divby>=10){
			nextLetter=' ';} else {padding=1;}
		lcd_putc(nextLetter);
		val-=(val/divby)*divby;
		divby/=10;
	}
}

void sendInt(unsigned int val){
	// send a number as ASCII text
	unsigned int divby=10000;
	while (divby>=1){
		lcd_putc('0'+val/divby);
		val-=(val/divby)*divby;
		divby/=10;
	}
}

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

volatile unsigned int prog[8];
void getProg(){
	// pull the program memory from the pressure sensor
	uint8_t data[2];
	char i,j;
	for (i=0;i<8;i++){
		data[0]=160+i*2;
		i2c2_transmit(0xEE,data,1);
		i2c2_receive(0xEF,data,2);
		prog[i]=data[0];
		prog[i]*=256;
		prog[i]+=data[1];
	}
}


volatile unsigned long int pressure;
volatile unsigned long int pressure0;
volatile unsigned long int temperature;

void pressure_baseline(){
	// reset pressure baseline to this value
	pressure0=0;
	read();
}

int read(){
	// read and display pressure
	uint8_t data[3];
	
	// GET PRESSURE
	data[0]=72; // command 72 is "set register to pressure"
	i2c2_transmit(0xEE,data,1); 
	_delay_ms(10); // give it time to make a read
	data[0]=0; // command 0 is "ADC read"
	i2c2_transmit(0xEE,data,1); 
	i2c2_receive(0xEF,data,3);
	pressure=0;
	pressure+=data[0];
	pressure=pressure<<8;
	pressure+=data[1];
	pressure=pressure<<8;
	pressure+=data[2];
	if (pressure0==0){pressure0=pressure;}
	
	/*
	// GET TEMPERATURE
	data[0]=88; // command 88 is "set register to temperature"
	i2c2_transmit(0xEE,data,1); 
	_delay_ms(10); // give it time to make a read
	data[0]=0; // command 0 is "ADC read"
	i2c2_transmit(0xEE,data,1); 
	i2c2_receive(0xEF,data,3);
	temperature=0;
	temperature+=data[0];
	temperature=temperature<<8;
	temperature+=data[1];
	temperature=temperature<<8;
	temperature+=data[2];
	*/
}

void showProg(){
	char i;
	lcd_puts("PROGRAM MEMORY:");
	for (i=0;i<6;i++){
		lcd_home();
		lcd_putc('\n');
		lcd_putc('C');
		lcd_putc('1'+i);
		lcd_putc('=');
		sendInt(prog[i+1]);
		_delay_ms(200);
	}
    lcd_init(LCD_DISP_ON);
}

void showRaw(){
	// DISPLAY RAW DATA ON DISPLAY
	lcd_gotoxy(0, 0);
	lcd_puts("P=");
	sendNum(pressure);
	lcd_gotoxy(0, 1);
	lcd_puts("T=");
	sendNum(temperature);
}

void showPressure(){
	if (pressure0==0){pressure0=pressure;}
	lcd_gotoxy(0, 0);
	//lcd_puts("P=");
	//sendNum(pressure);
	if (pressure>=pressure0){
		lcd_puts("\nd= +");
		sendInt(pressure-pressure0);
	} else {
		lcd_puts("\nd= -");
		sendInt(pressure0-pressure);
	}
	
		
}

void showReal(){

	unsigned long long int PSI;
	unsigned int PSIthou;
	unsigned int PSIfrac;
	char sign;
	
	// CALCULATE THE PRESSURE
	if (pressure>=pressure0){
		sign='+';
		PSI=pressure-pressure0;
	} else {
		sign='-';
		PSI=pressure0-pressure;
	}
	PSI*=1000;
	PSI/=318169;
	PSIthou=PSI/1000;
	PSI-=PSIthou*1000;
	PSIfrac=PSI;
	
	// DISPLAY THE PRESSURE
	lcd_gotoxy(0, 0);
	lcd_putc(sign);
	lcd_putc(sign);
	lcd_putc(' ');
	sendInt2(PSIthou,1);
	lcd_putc('.');
	sendInt3(PSIfrac,1);
	//sendNum((unsigned long int) PSI);
	lcd_puts(" PSI ");
	lcd_putc(sign);
	lcd_putc(sign);
	
	// ADD BAR GRAPH
	lcd_putc('\n');
	unsigned long segmentsOn;
	segmentsOn=PSIfrac*16/1000;
	int i;
	for(i=0;i<16;i++){
		if (i<=segmentsOn) {lcd_putc(255);}
		else {lcd_putc(' ');}
	}
	
	
	
}

int main(void)
{
    lcd_init(LCD_DISP_ON);
	lcd_led(1);
    lcd_home();
	
	getProg();
	showProg();
	pressure_baseline();
	
	DDRB&=~(1<<PB0); // make PB0 an input
	PORTB|=(1<<PB0); // pull our button high
	
    for(;;) {	
		read();
		//showRaw();
		showReal();
		
		if(~PINB&(1<<PB0)){
			lcd_gotoxy(0, 0);
			lcd_puts(" BASELINE RESET ");
			_delay_ms(500);
			pressure_baseline();
		}
		while (~PINB&(1<<PB0)){}
    }
}


