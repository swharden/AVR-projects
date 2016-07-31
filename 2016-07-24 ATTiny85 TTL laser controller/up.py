"""python3 control of buspirate"""

import serial

BUSPIRATE_PORT = 'com3' #customize this! Find it in device manager.

def send(ser,cmd):
    """send the command and listen to the response."""
    ser.write(str(cmd+'\n').encode('ascii')) # send our command
    for line in ser.readlines(): # while there's a response
        print(line.decode('utf-8').strip()) # show it

ser=serial.Serial(BUSPIRATE_PORT, 115200, timeout=1) # is com free?
assert ser.isOpen() #throw an exception if we aren't connected
send(ser,'#') # reset bus pirate (slow, maybe not needed)
send(ser,'m') # change mode (goal is to get away from HiZ)
send(ser,'9') # mode 9 is DIO
send(ser,'W') # turn power supply to ON. Lowercase w for OFF.
send(ser,'v') # show current voltages
ser.close() # disconnect so we can access it from another app
print("disconnected!") # let the user know we're done.