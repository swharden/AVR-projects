import sys
sys.path.append("../")
import buspirate
import os
import time

def getPressure():
    BP.command("[0xEE 72]",silent=True) # set register
    BP.command("[0xEE 0]",silent=True) # ADC read
    return BP.i2c_read(3,'0xef',silent=True)
    

def getTemp():
    BP.command("[0xEE 88]",silent=True) # set register
    BP.command("[0xEE 0]",silent=True) # ADC read
    return BP.i2c_read(3,'0xef',silent=True)

def getPROM():
    PROM=[None]*7
    for i in range(len(PROM)):
        BP.command("[0xEE %d]"%(160+i*2),silent=True)
        PROM[i]=BP.i2c_read(2,'0xEF',silent=True)
    return PROM
    
if __name__=="__main__":
    with open("data.csv",'w') as f:
        f.write('')
    BP=buspirate.BusPirate()
    BP.i2c_setup()
    PROM=getPROM()
    while True:
        try:
            TEMP=getTemp()
            PRES=getPressure()
            dT=TEMP-PROM[5]*(2**8)
            TEMP=(2000+dT*PROM[6]/(2**23))/100
            pOff=PROM[2]*2**16+(PROM[4]*dT)/2**7
            sens=PROM[1]*2**15+(PROM[3]*dT)/2**8
            PRES=((PRES*sens/2**21-pOff)/2**15)/100
            print()
            print("TEMP: %.02f C"%TEMP)
            print("PRES: %.02f mbar"%PRES)
            with open("data.csv",'a') as f:
                f.write("%.02f,%.02f,%.02f\n"%(time.time(),TEMP,PRES))
        except:
            print("FAILED TO READ!")
            time.sleep(1)
    BP.disconnect()