# Button Press Launches Interrupt
Here a button (pulled high, grounded when pressed) causes an interrupt.

```C

#include <avr/interrupt.h>

ISR(INT0_vect){
    // runs when the button is pressed
}

void SetupButton(){
  MCUCR=(1<<ISC01); // falling edge of INT0 generates interrupt
  GIMSK=(1<<INT0); // INT0 causes interrupt request
  EIFR=(1<<INTF0); // INT0 change Interrupt Flag
  PCMSK=(1<<PCINT1); // watch these pins
  sei(); // enable global interrupts
}


int main(void){
    SetupButton();
    for(;;){}
}
```
