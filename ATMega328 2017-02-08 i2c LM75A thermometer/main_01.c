#define F_CPU 1000000ul
#include <avr/io.h>
#include <util/delay.h>
#include "serial_m328.c"


void TWIInit(void)
{
    //set SCL to 400kHz
    TWSR = 0x00;
    TWBR = 0x0C;
    //enable TWI
    TWCR = (1<<TWEN);
}

void TWIStart(void)
{
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);
}
//send stop signal
void TWIStop(void)
{
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

void TWIWrite(uint8_t u8data)
{
    TWDR = u8data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);
}


uint8_t TWIReadACK(void)
{
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    while ((TWCR & (1<<TWINT)) == 0);
    return TWDR;
}
//read byte with NACK
uint8_t TWIReadNACK(void)
{
    TWCR = (1<<TWINT)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);
    return TWDR;
}

uint8_t TWIGetStatus(void)
{
    uint8_t status;
    //mask status
    status = TWSR & 0xF8;
    return status;
}

uint8_t test()
{
    //uint8_t databyte;
    TWIStart();
    if (TWIGetStatus() != 0x08)
        return 1;
			
    //select devise and send A2 A1 A0 address bits
    TWIWrite((EEDEVADR)|((uint8_t)((u16addr & 0x0700)>>7)));
    if (TWIGetStatus() != 0x18)
        return ERROR;
			
    //send start
    TWIStart();
    if (TWIGetStatus() != 0x10)
        return 2;
	
    //select devise and send read bit
    TWIWrite(0);
    if (TWIGetStatus() != 0x40)
        return 3;
	
	// attempt to return info
    return TWIReadNACK();
	
	// return error
    if (TWIGetStatus() != 0x58)
        return 4;
    TWIStop();
    return 5;
}

int main(void){
	serial_init();
	TWIInit();
	for(;;){
		_delay_ms(200);
		serial_number(test());
		serial_break();				
	}	
}