import matplotlib
matplotlib.use('TkAgg') # -- THIS MAKES IT FAST!
import numpy
import pyaudio
import threading
import pylab
import scipy
import time
import sys 


class SwhRecorder:
    """Simple, cross-platform class to record from the microphone.
    This version is optimized for SH-RFP (Scott Harden RF Protocol) 
    Pulse data extraction. It's dirty, but it's easy and it works.

    BIG PICTURE:
    continuously record sound in buffers.
    if buffer is detected:
        
        ### POPULATE DELAYS[] ###
        downsample data
        find Is where data>0
        use ediff1d to get differences between Is
        append >1 values to delays[]        
        --if the old audio[] ended high, figure out how many
        --on next run, add that number to the first I
        
        ### PLUCK DELAYS, POPULATE VALUES ###
        only analyze delays through the last 'break'
        values[] is populated with decoded delays.
        
    
    ."""
    
    def __init__(self):
        """minimal garb is executed when class is loaded."""
        self.RATE=44100
        self.BUFFERSIZE=2**10
        print "BUFFER:",self.BUFFERSIZE
        self.threadsDieNow=False
        self.newAudio=[]
        self.lastAudio=[]
        self.SHRFP=True
        self.dataString=""
        self.LEFTOVER=[]
        
        self.pulses=[]
        self.pulsesToKeep=1000
        
        self.data=[]
        self.dataToKeep=1000

        self.SIZE0=5
        self.SIZE1=10
        self.SIZE2=15
        self.SIZEF=3
        
        self.totalBits=0
        self.totalNumbers=0
        self.totalSamples=0
        self.totalTime=0
        
        self.nothingNewToShow=True
        
    def setup(self):
        """initialize sound card."""
        #TODO - windows detection vs. alsa or something for linux
        #TODO - try/except for sound card selection/initiation       
        self.p = pyaudio.PyAudio()
        self.inStream = self.p.open(input_device_index=None,
                                    format=pyaudio.paInt16,channels=1,
                                    rate=self.RATE,input=True,
                                    frames_per_buffer=self.BUFFERSIZE)
   
    
    def close(self):
        """cleanly back out and release sound card."""
        self.p.close(self.inStream)
    
    def decodeBit(self,s):
        "given a good string 1001101001 etc, return number or None"
        if len(s)<2:return -2
        s=s[::-1]
        A=s[:len(s)/2] #INVERTED
        A=A.replace("0","z").replace("1","0").replace("z","1")
        B=s[len(s)/2:] #NORMAL

        if A<>B:
            return -1
        else:
            return int(A,2)
    
    def analyzeDataString(self):
        i=0
        bit=""
        lastB=0
        while i<len(self.dataString):
            if self.dataString[i]=="B":
                self.data.append(self.decodeBit(bit))
                self.totalNumbers+=1
                lastB=i
            if self.dataString[i] in ['B','?']:
                bit=""
            else:
                bit+=self.dataString[i]
            i+=1
        self.dataString=self.dataString[lastB+1:]
        if len(self.data)>self.dataToKeep:
            self.data=self.data[-self.dataToKeep:]
    
    def continuousAnalysis(self):
        """keep watching newAudio, and process it."""
        while True:
            while len(self.newAudio)< self.BUFFERSIZE:
                time.sleep(.1)
            
            analysisStart=time.time()
            
            audio=self.newAudio
            
            # TODO - insert previous audio sequence here
            
            # GET Is where data is positive        
            Ipositive=numpy.nonzero(audio>0)[0]
            diffs=numpy.ediff1d(Ipositive)
            Idiffs=numpy.where(diffs>1)[0]
            Icross=Ipositive[Idiffs]
            pulses=diffs[Idiffs]            
                        
            # remove some of the audio buffer, leaving the overhang
            
            if len(Icross)>0:
                processedThrough=Icross[-1]+diffs[Idiffs[-1]]
            else:
                processedThrough=len(audio)

            self.lastAudio=self.newAudio[:processedThrough]            
            self.newAudio=self.newAudio[processedThrough:]
    
            if False:
                # chart audio data (use it to check algorythm)
                pylab.plot(audio,'b')
                pylab.axhline(0,color='k',ls=':')
                
                for i in range(len(Icross)):
                    # plot each below-zero pulse whose length is measured
                    pylab.axvspan(Icross[i],Icross[i]+diffs[Idiffs[i]],
                                  color='b',alpha=.2,lw=0)
                                  
                # plot the hangover that will be carried to next chunk
                pylab.axvspan(Icross[i]+diffs[Idiffs[i]],len(audio),
                              color='r',alpha=.2)
                pylab.show()
                return
            
            # TODO - histogram of this point to assess quality
            s=''        
            for pulse in pulses:
                if (self.SIZE0-self.SIZEF)<pulse<(self.SIZE0+self.SIZEF):
                    s+="0"
                elif (self.SIZE1-self.SIZEF)<pulse<(self.SIZE1+self.SIZEF):
                    s+="1"
                elif (self.SIZE2-self.SIZEF)<pulse<(self.SIZE2+self.SIZEF):
                    s+="B"
                else:
                    s+="?"

            self.pulses=pulses  
            self.totalBits+=len(pulses)         

            print "[%.02f ms took %.02f ms] T: 0=%d 1=%d B=%d ?=%d"%(
                          len(audio)*1000.0/self.RATE,
                          time.time()-analysisStart,
                          s.count('0'),s.count('1'),s.count('B'),s.count('?'))
            
            self.dataString+=s            
            self.analyzeDataString()

            self.totalSamples+=self.BUFFERSIZE
            self.totalTime=self.totalSamples/float(self.RATE)   
            self.totalBitRate=self.totalBits/self.totalTime  
            self.totalDataRate=self.totalNumbers/self.totalTime                   

            self.nothingNewToShow=False

    def continuousRecord(self):
        """record forever, adding to self.newAudio[]. Thread this out."""
        while self.threadsDieNow==False:
            maxSecBack=5
            while len(self.newAudio)>(maxSecBack*self.RATE):
                print "DELETING NEW AUDIO!"
                self.newAudio=self.newAudio[self.BUFFERSIZE:]
            audioString=self.inStream.read(self.BUFFERSIZE)
            audio=numpy.fromstring(audioString,dtype=numpy.int16)
            self.newAudio=numpy.hstack((self.newAudio,audio))


    def continuousDataGo(self):
        self.t = threading.Thread(target=self.continuousRecord)
        self.t.start()
        self.t2 = threading.Thread(target=self.continuousAnalysis)
        self.t2.start()
        
    def continuousEnd(self):
        """shut down continuous recording."""
        self.threadsDieNow=True

   


if __name__ == "__main__":
    SHR=SwhRecorder()
    SHR.SHRFP_decode=True
    SHR.setup()
    SHR.continuousDataGo()

    #SHR.DataStart()
    
    
    print "---DONE---"