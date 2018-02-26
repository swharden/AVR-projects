/*
* ----------------------------------------------------------------------------
* “THE COFFEEWARE LICENSE” (Revision 1):
* <ihsan@kehribar.me> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a coffee in return.
* -----------------------------------------------------------------------------
*/

#define F_CPU 8000000UL

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "radioPinFunctions.c"
#include "nrf24.h"
#include "nrf24.c"

uint8_t temp;
uint8_t q = 0;
uint8_t data_array[4] = {1,2,3,4};
uint8_t address_A[5] = {0xE7,0xE7,0xE7,0xE7,0xE7}; // will be radio-to-PC
uint8_t address_B[5] = {0xD7,0xD7,0xD7,0xD7,0xD7}; // will be PC-to-radio

void LEDblink(){
    PORTB|=(1<<PB1);
    _delay_ms(50);
    PORTB&=~(1<<PB1);
    _delay_ms(50);
}

int main()
{
    DDRB|=(1<<PB1); // set PB1 as output (pin 15) for LED

    nrf24_init();    
    nrf24_config(2,4); // Channel #2 , payload length: 4

    nrf24_tx_address(address_A);
    nrf24_rx_address(address_B);    

    LEDblink();

    while(1)
    {              
        if(nrf24_dataReady())
        {
            nrf24_getData(data_array);    
            char i=data_array[0];
            while (i-->0) LEDblink();
        }
    }
}
