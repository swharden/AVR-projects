# AVR pin noise
Sometimes I like to enable CKOUT (by setting a fuse). When I do this, I put ripples in the output current of all my other pins as well as the power supply. I practiced today building different circuits until I got one that's a good go-to for me. It lets me both isolate and buffer high frequency CKOUT signals while maintaining pure signals from slower changing pins.

## experiment
This is the voltage on a 20Hz output pin with 20MHz CKOUT (runnnig but not sourcing or sinking any current):

![](20hzRaw.jpg)

After adding a ferrite bead and capacitor, the 20MHz is filtered out. Note the rebound/rining that exists. Although I could filter more aggressively to eliminate that, I might be preventing the "sharpness" of my rise. Since my intent is to use this rise to drive an all-or-nothing buffer input (through a resistor no less), I'm satisfied with its shape.

![](20hzFerrite.jpg)

This is the test rig I made. I paid very close attention to good grounding and clean power supply (keeping ripple out).

![](build.jpg)

The schematic is more or less like this.

![](schem.jpg)
