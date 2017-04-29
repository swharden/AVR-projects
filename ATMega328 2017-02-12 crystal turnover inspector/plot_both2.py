import numpy as np
np.set_printoptions(precision=3, suppress=True)
import matplotlib.pyplot as plt
import time
import glob
import os

def smooth(data,sigma):
    """minimal complexity Gaussian smoothing."""
    points=np.exp(-np.power(np.arange(sigma)-sigma/2,2)/(2*np.power(sigma,2)))
    kernel=points/sum(points)
    pad=np.ones(int(len(kernel)/2))
    data=np.concatenate((pad*data[0],data,pad*data[-1]))
    data=np.convolve(data,kernel,mode='same')
    data=data[len(pad):-len(pad)]
    return data
    
def loadTempFreq(fname="data.csv",smoothTemp=60,smoothFreq=60,startAt=0):
    """
    given a log file (data.csv) return 2 lists:
      temperature (C) and frequency (Hz).
    This assumes every data point is 1sec apart.
    """
    with open(fname) as f:
        temps,freqs=[],[]
        for i,line in enumerate(f.read().split("\n")):
            if line.startswith("#") or len(line)<3:
                continue
            try:
                line=[float(x) for x in line.split(",")]
                assert len(line)==3
                freqs.append(line[1])
                temps.append(line[2])
            except:
                print("failed line",i,'=',line)
    if smoothTemp:
        temps=smooth(temps,smoothTemp)
    if smoothFreq:
        freqs=smooth(freqs,smoothFreq)
    #temps=temps*9/5+32 # convert from C to F
    if startAt:
        temps,freqs=temps[startAt:],freqs[startAt:]
        freqs-=min(freqs)
    #error checking
#    freqs[abs(freqs-freqs[0])>100]=np.nan
    return temps,freqs
            
def plot_add_circle(temps,freqs):
    times=np.arange(len(temps))/60 # minutes
    plt.scatter(temps,freqs,c=times,cmap='jet',lw=0,marker='.')   
    plt.colorbar(label="experiment time (minutes)")

def figure_derivative():
    plt.figure(figsize=(8,6))
    plt.grid()
    for fname in sorted(glob.glob("exp02 - 10.140/*.csv")):
        temps,freqs=loadTempFreq(fname,startAt=60*20)
        dfreqs=smooth(np.diff(freqs),60)*60 # Hz/min
        msg=" turnover at %.02f C"%temps[np.where(dfreqs>0)[0][0]]
        #plt.plot(temps,freqs,lw=3,alpha=.5,label=os.path.basename(fname)+msg)   
        plt.plot(temps[1:],dfreqs,'-',lw=1,alpha=.5,label=os.path.basename(fname)+msg)   
        plt.axhline(0,color='k',alpha=.2)
    plt.xlabel("temperature (C)")
    plt.ylabel("frequency (Hz)")
    plt.title("freqency vs temperature (10.140 MHz crystal)")
    plt.legend(fontsize=10,loc='upper left')
    plt.margins(.1,.1)
    #plt.axis([170,185,-2,2])
    plt.tight_layout()
    plt.show()    
    
def figure_circle(experimentPath='./'):
    plt.figure(figsize=(8,6))
    plt.grid()
    for fname in sorted(glob.glob(experimentPath+"/*.csv")):
        #temps,freqs=loadTempFreq(fname,startAt=60*20)
        temps,freqs=loadTempFreq(fname)
        freqs-=min(freqs)
        plot_add_circle(temps,freqs)
        #plt.plot(temps,freqs,lw=3,alpha=.5,label=os.path.basename(fname))   
        #plt.axhline(0,color='k',alpha=.2)
    plt.xlabel("temperature (C)")
    plt.ylabel("frequency (Hz)")
    plt.title("freqency vs temperature (10.140 MHz crystal)")
    plt.legend(fontsize=10,loc='upper left')
    plt.margins(.1,.1)
    #plt.axis([170,185,-2,2])
    plt.tight_layout()
    plt.show()    
    
def figure_linear(experimentPath='./'):
    plt.figure(figsize=(8,6))
    
    for subplot in [211,212]:
        plt.subplot(subplot)
        plt.grid()
        for vline in [60*10,60*70]:
            plt.axvline(vline/60,color='r',lw=3,alpha=.5,ls='--')
            
    for fname in sorted(glob.glob(experimentPath+"/*.csv")):
        temps,freqs=loadTempFreq(fname,startAt=60*20)
        #temps,freqs=loadTempFreq(fname)
        times=np.arange(len(temps))/60 # minutes
        plt.subplot(211)
        plt.plot(times,temps,lw=1,alpha=.5,label=os.path.basename(fname))   
        plt.subplot(212)
        plt.plot(times,freqs,lw=1,alpha=.5,label=os.path.basename(fname))  
    
    plt.subplot(211)
    plt.title("freqency and temperature vs time (10.140 MHz crystal)")
    plt.ylabel("temperature (F)")
    plt.legend(fontsize=10)
    plt.margins(0,.1)
    plt.subplot(212)
    plt.ylabel("frequency (Hz)")
    plt.xlabel("time (min)")
    plt.margins(0,.1)
    plt.tight_layout()
    plt.show() 
    
if __name__=="__main__":
    #figure_linear()
    while True:
        figure_circle()
#        break
        for i in range(20):
            print(i,end=" ")
            time.sleep(.5)
    print("DONE")