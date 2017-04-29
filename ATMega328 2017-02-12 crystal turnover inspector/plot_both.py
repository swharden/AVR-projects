import numpy as np
np.set_printoptions(precision=3, suppress=True)
import matplotlib.pyplot as plt
import time

SMOOTHBY_TEMP=60
SMOOTHBY_FREQ=10

def smooth(data,sigma):
    """minimal complexity Gaussian smoothing."""
    points=np.exp(-np.power(np.arange(sigma)-sigma/2,2)/(2*np.power(sigma,2)))
    kernel=points/sum(points)
    pad=np.ones(len(kernel)/2)
    data=np.concatenate((pad*data[0],data,pad*data[-1]))
    data=np.convolve(data,kernel,mode='same')
    data=data[len(pad):-len(pad)]
    return data

def fig_dual(times,temp,freq):
    
    freqOffset=freq[0]
    freq-=freqOffset
    
    plt.figure(figsize=(8,6))
    
    plt.subplot(211)
    plt.grid()
    plt.plot(times,temp,'.',color='#9999FF')
    plt.plot(times,smooth(temp,SMOOTHBY_TEMP),'k-',alpha=.5,lw=2)
    for t in [60*10,60*70]:
        if len(times)>t:
            plt.axvline(times[t],color='r',ls='--',lw=3,alpha=.2)
    plt.xlabel("experiment duration (minutes)")
    plt.ylabel("temperature (Fahrenheit)")
    plt.title("Temperature")
    plt.margins(0,.1)
    
    plt.subplot(212)
    plt.grid()
    plt.plot(times,freq,'.',color='#9999FF')
    plt.plot(times,smooth(freq,SMOOTHBY_FREQ),'k-',alpha=.5,lw=2)
    for t in [60*10,60*70]:
        if len(times)>t:
            plt.axvline(times[t],color='r',ls='--',lw=3,alpha=.2)
    plt.xlabel("experiment duration (minutes)")
    plt.ylabel("frequency (+%d)"%freqOffset)
    plt.title("Frequency")
    plt.margins(0,.1)
    
    plt.tight_layout()
    plt.show()
    
def fig_2d(times,temp,freq):
    freqOffset=freq[0]
    freq-=freqOffset

    plt.figure(figsize=(8,6))
    plt.grid()
    plt.scatter(smooth(temp,SMOOTHBY_TEMP),
                smooth(freq,SMOOTHBY_FREQ),
                c=times,cmap='jet',lw=0)
    plt.margins(.1,.1)
    plt.xlabel("temperature (F)")
    plt.ylabel("frequency (Hz +%d)"%freqOffset)
    plt.title("frequency vs temperature")
       
    plt.colorbar(label="experiment time (minutes)")
    plt.tight_layout()
    plt.show()
    
def doit(fnames=["data.csv"]):
    times,temp,freq=[],[],[]
    for fname in fnames:
        with open(fname) as f:
            for line in f.readlines():
                if "," in line and not line.startswith("#"):
                    try:
                        line=line.split(",")
                        tm,fq,tp=float(line[0]),float(line[1]),float(line[2])
                        if tp<100:
                            times.append(tm)
                            temp.append(tp)
                            freq.append(fq)
                    except:
                        print("line fail:",line)
                        
    times=np.arange(len(times))
    times=np.array(times)
    times=times-times[0]
    times=times/60.0
        
    for i in range(1,len(freq)):
        if abs(freq[i]-freq[i-1])>10:
            freq[i]=freq[i-1]
    
    temp=np.array(temp)
    temp=temp*9/5+32 # now in F
    
    fig_dual(times,np.array(temp),np.array(freq))
    fig_2d(times,np.array(temp),np.array(freq))
                    
    print("DONE")
    
if __name__=="__main__":
    while True:
        print("\n"*50)
        #doit("exp01-nice/data.csv")
        doit()
#        break
        time.sleep(2)