import numpy as np
import matplotlib.pyplot as plt

def voltage2temp(v):   
    #R2=(R1*V2)/(V1-V2)
    v=(10000*v)/(5-v)
    # R to T for 10k NTC thermistor
    A = 1.12924E-03
    B = 2.34108E-04
    C = 0.87755E-07
    Kelvin=1/(A+B*np.math.log(v)+C*np.math.log(v)**3) # kelvin
    celsius=Kelvin-273.15 # now in celsius 
    return celsius
    
if __name__=="__main__":
    voltages=np.linspace(.2,4.5,1000)
    temps=voltages.copy()
    for i,v in enumerate(voltages):
        temps[i]=voltage2temp(v)
    print("DONE")
    
    plt.figure(figsize=(5,5))
    plt.grid()
    plt.plot(voltages,temps,'b-',lw=3,alpha=.5,label='celsius')
    plt.plot(voltages,temps*9/5+32,'r-',lw=3,alpha=.5,label='Fahrenheit')
    plt.legend(fontsize=12)
    plt.ylabel("Temperature")
    plt.xlabel("Divider Voltage (V2)")
    plt.title("(+5V)-[10k]-(V2)-[10k NTC]-(GND)")
    plt.axhspan(70,160,color='r',lw=0,alpha=.2)
    plt.margins(.1,.1)
    plt.tight_layout()