"""this script continuously polls for temperature and saves it as data.csv"""

import serial
import time
import serial.tools.list_ports
for item in serial.tools.list_ports.comports():
    print(item)
                    
BUSPIRATE_PORT = 'com3' #customize this! Find it in device manager.

def send(ser,cmd,silent=False):
    """
    send the command and listen to the response.
    returns a list of the returned lines. 
    The first item is always the command sent.
    """
    ser.write(str(cmd+'\n').encode('ascii')) # send our command
    lines=[]
    for line in ser.readlines(): # while there's a response
        lines.append(line.decode('utf-8').strip())
    if not silent:
        print("\n".join(lines))
        print('-'*60)
    return lines

def getTemp(ser,address='0x91',silent=True,fahrenheit=False):
    """return the temperature read from an LM75"""
    unit=" F" if fahrenheit else " C"
    lines=send(ser,'[%s r:2]'%address,silent=silent) # read two bytes
    for line in lines:
        if line.startswith("READ:"):
            line=line.split(" ",1)[1].replace("ACK",'')
            while "  " in line:
                line=" "+line.strip().replace("  "," ")
            line=line.split(" 0x")
            val=int("".join(line),16)
            # conversion to C according to the datasheet
            if val < 2**15:
                val = val/2**8
            else:
                val =  (val-2**16)/2**8
            if fahrenheit:
                val=val*9/5+32
            if not silent:
                print("%.03f"%val+unit)
            return val

# the speed of sequential commands is determined by this timeout
ser=serial.Serial(BUSPIRATE_PORT, 115200, timeout=.1)

# have a clean starting point
send(ser,'#',silent=True) # reset bus pirate (slow, maybe not needed)
#send(ser,'v') # show current voltages

# set mode to I2C
send(ser,'m',silent=True) # change mode (goal is to get away from HiZ)
send(ser,'4',silent=True) # mode 4 is I2C
send(ser,'3',silent=True) # 100KHz
send(ser,'W',silent=True) # turn power supply to ON. Lowercase w for OFF.
send(ser,'P',silent=True) # enable pull-up resistors
send(ser,'(1)') # scan I2C devices. Returns "0x90(0x48 W) 0x91(0x48 R)"

#fname=str(int(time.time()))+".csv"
fname='data.csv'
t1=time.time()
with open(fname,'w') as f:
    f.write("#time, temperature\n")
with open(fname,'a') as f:
    nReads=0
    try:
        print("reading data until CTRL+C is pressed...")
        while True:
            line="%.02f, %.03f\n"%(time.time()-t1,getTemp(ser,fahrenheit=True))
            print(line.strip())
            f.write(line)
            nReads+=1
            if nReads%10==1:
                print("writing",fname,'...')
                f.flush()
    except:
        print("exception broke continuous reading.")

print("data saved in",fname)

ser.close() # disconnect so we can access it from another app
print("disconnected!") # let the user know we're done.