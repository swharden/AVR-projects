import numpy as np
np.set_printoptions(precision=3, suppress=True)
import matplotlib.pyplot as plt

def smooth(data,sigma=60):
    """minimal complexity Gaussian smoothing."""
    points=np.exp(-np.power(np.arange(sigma)-sigma/2,2)/(2*np.power(sigma,2)))
    kernel= points/sum(points)
    pad=np.ones(len(kernel)/2)
    data=np.concatenate((pad*data[0],data,pad*data[-1]))
    data=np.convolve(data,kernel,mode='same')
    data=data[len(pad):-len(pad)]
    return data

if __name__=="__main__":
    data=np.loadtxt("data.csv",delimiter=",")
    times=data[:,0]/60.0 # convert from seconds to minutes
    temp=data[:,1] # temperature (F)
    plt.figure(figsize=(8,4))
    plt.grid()
    plt.axvspan(320/60,626/60,color='b',alpha=.1,label="exposed 47k")
    plt.axvspan(1534/60,1692/60,color='g',alpha=.1,label="igloo 47k")
    plt.axvspan(2090/60,2300/60,color='r',alpha=.1,label="igloo 1k")
    plt.plot(times,temp,',',color='#9999FF')
    plt.plot(times,smooth(temp),'k-',alpha=.5,lw=2)
    plt.xlabel("experiment duration (minutes)")
    plt.ylabel("temperature (Fahrenheit)")
    plt.title("Heater test circuit (LM74 I2C thermometer / Bus Pirate)")
    plt.legend(fontsize=10,loc='upper left')
    plt.margins(0,.1)
    plt.tight_layout()
    print(data)
    print("DONE")