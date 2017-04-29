/*
lcdpcf8574 lib sample

copyright (c) Davide Gironi, 2013

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lcdpcf8574/lcdpcf8574.h"

#define UART_BAUD_RATE 2400
#include "uart/uart.h"

int main(void)
{
	//init uart
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );

    sei();

    //init lcd
    lcd_init(LCD_DISP_ON_BLINK);

    //lcd go home
    lcd_home();

    uint8_t led = 0;

    while(1) {
    	lcd_led(led); //set led
    	led = !led; //invert led for next loop

    	//test loop
    	int i = 0;
    	int line = 0;
    	for(i=0; i<10; i++) {
    		char buf[10];
    		itoa(i, buf, 10);
    		lcd_gotoxy(0, line);
    		lcd_puts("i= ");
    		lcd_gotoxy(3, line);
    		lcd_puts(buf);
    		line++;
    		line %= 2;
        	uart_puts(buf);
        	uart_puts("\r\n");
    		_delay_ms(500);
    	}
    }
}


