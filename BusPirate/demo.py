"""
dear god scott don't make a Python BusPirate API
"""

# develop entirely in 1 flat file to start with
# the purpose is *NOT* to use bit-bang mode! copy functionality
# of sitting there banging away at the serial terminal.

import time
import serial
import warnings

class BusPirate:
    
    def __init__(self,port=None,baud=115200):
        """
        Open a hardware connection with a Bus Pirate.
        If a device isn't given, the system serial ports will be scanned, and
        any USB serial port with a VID matching the Bus Pirate will be
        automatically selected. With only 1 Bus Pirate plugged in, this means
        you never have to know what port it's on.
        """
        if not port:
            port=self.portScan()
        self.port=port
        self.baud=baud
        self.ser=None
        self.connect()
        self.mainMenu()
        
    ### hardware actions
        
    def portScan(self):
        """
        Show every serial port on the system.
        Return the most likely Bus Pirate part (or None)
        """
        print("scanning COM ports...")
        import serial.tools.list_ports
        piratePort=None
        for port in serial.tools.list_ports.comports():
            print(" ",port)
            if port.vid and port.vid == 1027:
                piratePort=port.device
        if not piratePort:
            warnings.warn("could not find a Bus Pirate device")
        return piratePort
        
    def connect(self):
        """
        initialize a connection with the serial port.
        Don't forget to call close() when you're done
        """
        print("opening %s at %d baud..."%(self.port,self.baud))
        if not self.port:
            warnings.warn("no port defined, so not opening a connection.")
            return
        self.ser=serial.Serial(self.port, self.baud, timeout=1)
        
    def disconnect(self):
        """close the serial port."""
        if self.ser is None:
            return
        self.ser.close()
        print("disconnected from",self.port)
        
    ### interaction
        
    def command(self,command=None,showOutput=True):
        """
        Runs a command and listens for a response.
        We assume the response is over when a \nXXX> is detected.
        """
        self.ser.write(str(command+'\n').encode('ascii'))
        response=''
        t1=time.clock()
        while True:
            if time.clock()-t1>1:
                print("1 sec passed and no prompt was found...")
                break
            response+=self.ser.read(1).decode('utf-8').replace("\r","\n")
            if response[-1]==">" and '\n' in response[-6:-5]:
                break
        response=response.replace("\n\n","\n").split("\n")
        while "" in response:
            response.remove("")
        command,message,prompt=response[0],response[1:-1],response[-1]
        if showOutput:
            print("COMMAND:",command)
            #print("RESPONSE:\n")
            print("\n".join(message))
            print("PROMPT:",prompt)
            print('-'*60)
        return("\n".join(message))
        
    def commandSequence(self,commands,showOutput=True):
        """
        Run each command separated comma.
        Must start from main menu.
        """
        self.mainMenu()        
        if "," in commands:
            commands=commands.split(",")
        else:
            commands=[commands]
        for command in commands:
            self.command(command,showOutput=showOutput)
            
    ### commands (run all from home screen)
    
    def info(self):
        self.command("i")
        
    def mainMenu(self):
        """send 'i' until we return to the main menu."""
        for i in range(10):
            message=self.command("i",showOutput=False)
            if message.startswith("Bus Pirate"):
                print("### At the main menu of",message.split("\n")[0],"###")
                return
        print("PROBLEM! Can't get to main menu by pressing 'i'...")
        return
        
    def reset(self):
        """reset the device"""
        self.command("#")
        
    def mode_i2c(self,rate='3'):
        """switch to I2C mode (100khz)"""
        self.mainMenu()
        self.command("m")
        self.command("4")
        self.command("3")
        self.power()
        self.pullup()
        self.command("(1)") # scan addresses
        
    def i2c_read(self,numBytes=2,address='0x91',returnTotal=True):
        """
        Given an address and number of bytes, returns the bytes as integers.
        If addItUp is True, return all bytes as a single number.
        """
        data=self.command("[%s r:%d]"%(address,numBytes))
        data=data.split("READ:")[1].split("NACK")[0]
        data=data.replace("ACK","").split("0x")
        data=[x.strip() for x in data if len(x.strip())]
        data=[int(x,16) for x in data]
        total=0
        for byteNum,value in enumerate(data):
            total+=value*(2**(8*(len(data)-byteNum-1)))
        print("bytes: %s sum=%d"%(str(data),total))
        if returnTotal:
            return total
        return data
        
    def power(self,on=True):
        """turn PSU on/off"""
        if on:
            self.command("W")
        else:
            self.command("W")
            
    def pullup(self,high=True):
        """set pullup resistors high/low"""
        if high:
            self.command("P")
        else:
            self.command("p")
        
    def aux(self,high=True):
        """set aux pin high/low"""
        self.command("a")
        if high:
            self.command("A")
        else:
            self.command("a")
        
    def pwm(self,enable=True,duty=50,kHz=1):
        """
        enable/disable pwm at the given freq (1-4000 kHz) and duty (int).
        Duty needs to be >=2 and <=99
        """
        self.mainMenu()
        if "PWM disabled" in BP.command('g'):
            if enable:
                BP.command('g')
            else:
                return
        BP.command(str(kHz))
        BP.command(str(duty))
        return
        
if __name__=="__main__":
    BP=BusPirate()
    BP.mode_i2c()
    BP.i2c_read()
    BP.disconnect()
    print("DONE")
