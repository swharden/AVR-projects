# USART (serial) communication with ATMega328

### TX only
**[This example](tx%20rx%20interrupt/main.c) shows how to send data one way (AVR -> PC)**

### TX + RX (polling method)
**[This example](tx%20rx%20polling/main.c) shows how bidirectionally exchange data (AVR <-> PC) using a polling method.** This is typically less favorable than the interrupt method because the processing of the chip gets to a point where it waits (perhaps forever) until the PC sends data.

### TX + RX (polling interrupt method)
**[This example](tx%20rx%20interrupt/main.c) shows how bidirectionally exchange data (AVR <-> PC) using an interrupt method.** This is typically the best method because the MCU can run continuously doing its own thing, and the PC _interrupts_ the microcontroller when USART data begins to come in.

## Hardware
description | picture
---|---
I typically use an FT232 breakboard board to capture serial data from my microcontroller on my computer. These breakoutboards can be [purchased from SparkFun](https://www.sparkfun.com/products/12731) (which are always high quality and genuine) or [from eBay](http://www.ebay.com/sch/ft232+breakout) (cheaper, but variable quality, and questionable chips). | ![](../ATMega328%202017-02-08%20i2c%20LM75A%20thermometer/circuit.jpg)
On the software side, I typically use [RealTerm](https://realterm.sourceforge.io/). It's simple, easy, and allows file logging. If you configure your microcontroller to output CSV format, you can save directly as CSV so files can be opened in Excel. Nice!|![](../ATMega328%202017-02-08%20i2c%20LM75A%20thermometer/demo.png)

