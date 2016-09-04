# sn74lv8154 AVR counter
The sn74lv8154 is a dual 16-bit counter than can be configured to act as a 32-bit counter. Although the datasheet doesn't report functionality at VHF frequencies, in my tests it works at least at 100MHz. The way I operate it is as a frequency counter with a gate. I'm measuring unknown frequencies with 1 pulse per second gate (from a GPS unit).

* Datasheet: http://www.ti.com/lit/ds/symlink/sn74lv8154.pdf

This program measures frequency with the AVR and outputs frequency (counts) to a computer via the serial port. Included is a barebones python script to display these frequency counts.

![](idea.png)
![](ss.png)
