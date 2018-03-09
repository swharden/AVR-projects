/*
    TRANSMITTER - use a nrf24L01 to send data to the RX.
    Two LEDs are connected to PD6 (green) and PD7 (red) which indicate
    if the transmitted message was delivered successfully.
    The nrf24L01 [CE, CSN, SCK, MOSI, MISO] connects to [PD0, PD1, PD2, PD3, PD4]
*/

#define F_CPU 8000000UL

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "radioPinFunctions.c"
#include "nrf24.h"
#include "nrf24.c"

void waitMs1(){_delay_ms(1);}
void waitMs(int ms){while (ms-->0){waitMs1();}}
void waitSec(int sec){while (sec-->0){waitMs(1000);}}

volatile uint8_t data_array[3] = {1,1,1};
uint8_t address_Tx[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
uint8_t address_Rx[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};

void sendBytes(uint8_t b1, uint8_t b2, uint8_t b3){
    data_array[0]=b1;
    data_array[1]=b2;
    data_array[2]=b3;

    nrf24_send(data_array);  
    while(nrf24_isSending()) { /* */ }
    
    uint8_t status;
    status = nrf24_lastMessageStatus();
    if(status == NRF24_TRANSMISSON_OK) {setLights(1,0);}
    else if(status == NRF24_MESSAGE_LOST) {setLights(0,1);}
}

void setLights(uint8_t green, uint8_t red){
    DDRD|=(1<<PD6)|(1<<PD6);
    if (green>0) {PORTD|=(1<<PD6);} else {PORTD&=~(1<<PD6);}
    if (red>0) {PORTD|=(1<<PD7);} else {PORTD&=~(1<<PD7);}
}

int main()
{

    nrf24_init();    
    nrf24_config(2,sizeof(data_array)); // Channel and payload size  
    nrf24_tx_address(address_Rx);
    nrf24_rx_address(address_Tx);    

    setLights(1,1); waitMs(500);

    while(1)
    {
        sendBytes(0,0,0); waitMs(500);
        sendBytes(1,0,0); waitMs(500);
        sendBytes(0,1,0); waitMs(500);
        sendBytes(0,0,1); waitMs(500);
        sendBytes(1,1,1); waitMs(500);
    }
}
