# USART (serial) communication with ATMega328

### TX only
**[This example](tx%20only/main.c) shows how to send data one way (AVR -> PC)**
```C
void serial_send(unsigned char data){
	// send a single character via USART
	while(!(UCSR0A&(1<<UDRE0))){}; //wait while previous byte is completed
	UDR0 = data; // Transmit data
}
```

### TX + RX (polling method)
**[This example](tx%20rx%20polling/main.c) shows how bidirectionally exchange data (AVR <-> PC) using a polling method.** This is typically less favorable than the interrupt method because the processing of the chip gets to a point where it waits (perhaps forever) until the PC sends data.
```C
char serial_read()
{
   // wait until single character is read via USART
   while(!(UCSR0A & (1<<RXC0))){} // wait until data comes
   return UDR0; // return the character
}
```

### TX + RX (polling interrupt method)
**[This example](tx%20rx%20interrupt/main.c) shows how bidirectionally exchange data (AVR <-> PC) using an interrupt method.** This is typically the best method because the MCU can run continuously doing its own thing, and the PC _interrupts_ the microcontroller when USART data begins to come in.
```C
volatile char lastLetter;
ISR(USART_RX_vect){
    lastLetter=UDR0;
} // this runs when we capture
```

## Hardware
description | picture
---|---
I typically use an FT232 breakboard board to capture serial data from my microcontroller on my computer. These breakoutboards can be [purchased from SparkFun](https://www.sparkfun.com/products/12731) (which are always high quality and genuine) or [from eBay](http://www.ebay.com/sch/ft232+breakout) (cheaper, but variable quality, and questionable chips). | ![](../ATMega328%202017-02-08%20i2c%20LM75A%20thermometer/circuit.jpg)
On the software side, I typically use [RealTerm](https://realterm.sourceforge.io/). It's simple, easy, and allows file logging. If you configure your microcontroller to output CSV format, you can save directly as CSV so files can be opened in Excel. Nice!|![](../ATMega328%202017-02-08%20i2c%20LM75A%20thermometer/demo.png)

