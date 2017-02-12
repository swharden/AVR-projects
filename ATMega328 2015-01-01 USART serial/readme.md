# USART (serial) communication with ATMega328

description | picture
---|---
I typically use an FT232 breakboard board to capture serial data from my microcontroller on my computer. These breakoutboards can be [purchased from SparkFun](https://www.sparkfun.com/products/12731) (which are always high quality and genuine) or [from eBay](http://www.ebay.com/sch/ft232+breakout) (cheaper, but variable quality, and questionable chips). | ![](../ATMega328%202017-02-08%20i2c%20LM75A%20thermometer/circuit.jpg)
On the software side, I typically use [RealTerm](https://realterm.sourceforge.io/). It's simple, easy, and allows file logging. If you configure your microcontroller to output CSV format, you can save directly as CSV so files can be opened in Excel. Nice!|![](../ATMega328%202017-02-08%20i2c%20LM75A%20thermometer/demo.png)

## TX only
This example sends data one way (AVR -> PC)|![](tx%20only/demo.jpg)
---|---

## TX + RX (polling method)

## TX + RX (polling interrupt method)
This example sends data two ways (AVR <-> PC) using an interrupt method. This is usually the best way of doing this.|![]('tx%20rx%20interrupt/demo.png')
---|---

