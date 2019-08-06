import matplotlib
matplotlib.use('TkAgg') # -- THIS MAKES IT FAST!
import numpy
import pyaudio
import threading
import pylab
import time


class SwhRecorder:
    """Simple, cross-platform class to record from the microphone."""
    
    def __init__(self):
        """minimal garb is executed when class is loaded."""
        self.RATE=48100
        self.BUFFERSIZE=2**12 #1024 is a good buffer size
        self.secToRecord=.1
        self.threadsDieNow=False
        self.newAudio=False
        self.SHRFP_decode=True
        self.digitalAudio=True
        
        self.pulses=[]        
        self.data=[]
        self.LOCKED=0
        self.LOCK_COUNT=5 #number of consecutive steps required to program size
        self.LOCK_STEPS=3 #number of steps (I use 3: 0, 1, and break)
        self.LOCK_SIZE=self.LOCK_COUNT*self.LOCK_STEPS
        
    def setup(self):
        """initialize sound card."""
        #TODO - windows detection vs. alsa or something for linux
        #TODO - try/except for sound card selection/initiation

        self.buffersToRecord=int(self.RATE*self.secToRecord/self.BUFFERSIZE)
        if self.buffersToRecord==0: self.buffersToRecord=1
        self.samplesToRecord=int(self.BUFFERSIZE*self.buffersToRecord)
        self.chunksToRecord=int(self.samplesToRecord/self.BUFFERSIZE)
        self.secPerPoint=1.0/self.RATE
        
        self.p = pyaudio.PyAudio()
        self.inStream = self.p.open(format=pyaudio.paInt16,channels=1,
                                    rate=self.RATE,input=True,
                                    frames_per_buffer=self.BUFFERSIZE)
                                    
#        self.xs=numpy.arange(self.chunksToRecord*self.BUFFERSIZE)*self.secPerPoint
#        self.xs=self.xs*1000.0 #now in ms
#        self.audio=numpy.empty((self.chunksToRecord*self.BUFFERSIZE),dtype=numpy.int16)   
#        self.audio=numpy.empty((1,self.BUFFERSIZE)) #FILL THIS BUFFER UP!
#        self.audio[:]=numpy.nan
        self.audio=None
        print self.audio
    
    def close(self):
        """cleanly back out and release sound card."""
        self.p.close(self.inStream)
    
    
    
    ### RECORDING AUDIO ###  
    
    def getAudio(self):
        """get a single buffer size worth of audio."""
        audioString=self.inStream.read(self.BUFFERSIZE)
        audio=numpy.fromstring(audioString,dtype=numpy.int16)
        if self.digitalAudio: 
            audio=audio>0
            audio=audio.astype(numpy.int0)
        return audio
        
    def record(self,buffers=-1):
        """record secToRecord seconds of audio."""
        print " -- recording: started"
        while buffers<>0:
            
            # exit if unthreaded parent says so
            if self.threadsDieNow: 
                break

            # extend audio[] by recording data
            if self.audio==None:
                self.audio=self.getAudio()
            else:
                newAudio=self.getAudio()
                self.audio=numpy.hstack((self.audio,newAudio))
            self.newAudio=True
            self.SHRFP_checknew()
            buffers-=1
            
        print " -- recording: stopped"
        
#    def watchForNew(self):
#        while self.threadsDieNow==False:
#            time.sleep(.1)
#            self.SHRFP_checknew()
    
    def continuousStart(self):
        """CALL THIS to start running forever."""
        self.t = threading.Thread(target=self.record)
        self.t.start()
#        self.w = threading.Thread(target=self.watchForNew)
#        self.w.start()
        
    def continuousEnd(self):
        """shut down continuous recording."""
        self.threadsDieNow=True

    ### SHRFP ###
    
    def whichPulse(self, pulse):
        """Given a pulse length and lock lengths, find best match.
        
        return absolute difference if not exact.    
        """
        bestD=1000
        bestI=0
        for i in range(len(self.pulseLengths)): 
            diff=numpy.abs(self.pulseLengths[i]-pulse)
            if diff<bestD: 
                bestD=diff
                bestI=i
        return bestI,bestD
        
    def whichPulses(self, pulses):
        Is=[]
        for i in range(len(pulses)):
            Is.append(self.whichPulse(pulses[i])[0])
        return Is

    def SHRFP_islocked(self,data):
        # reshape data until theoretical steps
        data=numpy.reshape(data,(len(data)/self.LOCK_STEPS,self.LOCK_STEPS))
        
        # make sure each number is larger than the previous
        for line in data:
            if line[1]<line[0] or line[2]<line[1]: 
                #print "FAIL"
                return False
                
        # if we got this far, this is a theoretical lock. measure variability.
        maxSD=numpy.max(numpy.std(data,axis=0))
        if maxSD>1: 
            return False
        
        # now do stats and return integer averages
        self.pulseLengths=numpy.average(data,axis=0)
        
        if numpy.std(self.pulseLengths)<2:
            return False
        
        return True
        
    def SHRFP_handle(self,points):        
        points=points[:-1]
        points=points[::-1]
        A=points[:len(points)/2] #INVERTED
        A=A.replace("0","z").replace("1","0").replace("z","1")
        B=points[len(points)/2:] #NORMAL
        if A<>B:
            self.stats_badpackets+=1
            self.data.append(None)
        else:
            self.stats_goodpackets+=1
            self.data.append(int(A,2))
            
        if len(self.data)>1000: 
            self.data=self.data[-1000:]

    def SHRFP_newPulse(self):
        if self.LOCKED==0:
            while True:
                if len(self.pulses)<self.LOCK_STEPS*self.LOCK_COUNT: return
                if self.SHRFP_islocked(self.pulses)==False:
                    self.pulses.pop(0)
                    #print "NO LOCK"
                else:
                    self.pulses=self.pulses[self.LOCK_STEPS*self.LOCK_COUNT:]
                    print "GOT LOCK!"
                    self.stats_badpackets=0
                    self.stats_goodpackets=0
                    print self.pulseLengths
                    self.LOCKED+=self.LOCK_COUNT
                    break
            
        
        ### ALREADY LOCKED, ACCUMULATE LOCK BITS
        while self.LOCKED>=0:
            if len(self.pulses)<self.LOCK_STEPS: return
            if self.whichPulses(self.pulses[:self.LOCK_STEPS])==range(self.LOCK_STEPS):
                self.pulses=self.pulses[self.LOCK_STEPS:]
                self.LOCKED+=1
            else:
                self.pulses=self.pulses[self.LOCK_STEPS:]
                self.LOCKED*=-1                
                self.bits=""
                print "LOCK COMPLETE:",abs(self.LOCKED)
                self.data=[]
                break

        if self.LOCKED<0:
            ### LOCKED, NOW ACCUMULATE DATA BITS
            self.bits+=''.join(str(e) for e in self.whichPulses(self.pulses))
            self.pulses=[]
            if len(self.bits)==0: return
            
            ### LOCKED, DATA BITS OVER, PROCESS NUMBER        
            if self.bits[-1]=="2":
                # reset character buffer
                points=self.bits
                self.bits=""
                if len(points)==1: #double 2s means line break
                    self.data.append("BREAK")
                else: # we landed a number, handle it!
                    self.SHRFP_handle(points)
        
        return

    def SHRFP_checknew(self):
        """figure out what to do with the new data."""
        
        if self.newAudio==False: 
            #nothing new to check
            return
        
        self.newAudio=False
        i=0
        
        pulsesValid=0
        pulsesInvalid=0
        
        FUDGE_HL=4 #acceptable difference between high and low time
        
        # figure out the range of logical data to record
        startAt=0
        endAt=len(self.audio)-1
        while self.audio[startAt]==1: 
            startAt+=1 #ignore region that starts high
        while self.audio[endAt]==0: 
            endAt-=1 #ignore region at end if it's low
        #print "START/END:",startAt,endAt
        
        # cycle through linear audio, sending pulses as they're found    
        i=startAt
        while i<endAt:
            # FIND PULSE LOW AND HIGH
            pulseLow=0
            pulseHigh=0
            while i<endAt and self.audio[i]==0:
                pulseLow+=1
                i+=1
            while i<endAt and self.audio[i]==1:
                pulseHigh+=1
                i+=1
            if abs(pulseLow-pulseHigh)<FUDGE_HL:
                pulsesValid+=1
                self.pulses.append(pulseLow+pulseHigh)
                self.SHRFP_newPulse()
            else:
                pulsesInvalid+=1
        
        # delete audio that has been processed
        #print "CUTTING TO:",endAt
        self.audio[endAt:]
                
#        print "ERRORS: %d of %d (%.04f%%)"%(pulsesInvalid,  
#                            pulsesInvalid+pulsesValid,
#                            pulsesInvalid*100.0/(pulsesInvalid+pulsesValid))
        
        return
    
    ### MATH ###
            
    def downsample(self,data,mult):
        """Given 1D data, return the binned average."""
        overhang=len(data)%mult
        if overhang: data=data[:-overhang]
        data=numpy.reshape(data,(len(data)/mult,mult))
        data=numpy.average(data,1)
        return data    
        
    def fft(self,data=None,trimBy=10,logScale=False,divBy=100):
        if data==None: 
            data=self.audio.flatten()
        left,right=numpy.split(numpy.abs(numpy.fft.fft(data)),2)
        ys=numpy.add(left,right[::-1])
        if logScale:
            ys=numpy.multiply(20,numpy.log10(ys))
        xs=numpy.arange(self.BUFFERSIZE/2,dtype=float)
        if trimBy:
            i=int((self.BUFFERSIZE/2)/trimBy)
            ys=ys[:i]
            xs=xs[:i]*self.RATE/self.BUFFERSIZE
        if divBy:
            ys=ys/float(divBy)
        return xs,ys
    
    ### VISUALIZATION ###
    
    def plotAudio(self):
        """open a matplotlib popup window showing audio data."""
        pylab.plot(self.audio.flatten())
        pylab.show()        


if __name__ == "__main__":
    SHR=SwhRecorder()
    SHR.SHRFP_decode=True
    SHR.setup()
    #SHR.continuousStart()
    SHR.record(10)
    SHR.SHRFP_checknew()
    print SHR.data
    #print SHR.audio
    print "---DONE---"