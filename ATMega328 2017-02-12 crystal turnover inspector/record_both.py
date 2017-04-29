"""this script continuously polls for temperature and saves it as data.csv"""

import serial
import time
import serial.tools.list_ports
import os

def listPorts():
    for item in serial.tools.list_ports.comports():
        print(item)
        
def getFreq(ser):
    while True:
        try:
            freq=ser.readline().decode('utf-8').strip()
            if freq.startswith("F="):
                freq=freq[2:]
                return int(freq)
        except:
            print("frequency reading failed!")

def getTemp(ser):
    while True:
        try:
            ser.write('r'.encode('ascii'))
            temp=ser.readline().decode('utf-8').strip()
            #print("TEMP:",temp)
            temp=float(temp)
            return temp
        except:
            print("temperature reading failed!")

def logLine(line,fname="data.csv",firstLine=False):
    if type(line) == list:
        line=[str(x) for x in line]
        line=",".join(line)
    if firstLine:
        with open(fname,'w') as f:
            f.write("# "+line)
    with open(fname,'a') as f:
        f.write(line+"\n")
            
if __name__=="__main__":
        
    
    logLine(["time","freq","temp"],firstLine=True)
    
    serTemp=serial.Serial('COM51', 4800, timeout=2)
    serFreq=serial.Serial('COM52', 4800, timeout=2)
    
    nReads=0
    while True:
        try:
            freq=getFreq(serFreq)
            temp=getTemp(serTemp)
            tnow="%.02f"%time.time()
            logLine([tnow,freq,temp])
            nReads+=1
            print([nReads,tnow,freq,temp])
            time.sleep(.5)
            
            if nReads==60*10: # 10 minute mark
                print("HEATER ON")
                serTemp.write('o'.encode('ascii'))
            if temp>=100 or nReads==60*70: # 10 minutes + 1 hour of heating
                print("HEATER OFF")
                serTemp.write('f'.encode('ascii'))
            
        except:
            print("EXCEPTION!")
            break
        
    print("disconnecting...")
    serTemp.close()
    serFreq.close()
    print("DONE")