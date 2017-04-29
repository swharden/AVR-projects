import numpy as np
np.set_printoptions(precision=3, suppress=True)
import matplotlib.pyplot as plt
import time

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
    while True:

        #data=np.loadtxt("data_temp.csv",delimiter=",")
        #times=data[:,0]
        #temp=data[:,1]
        times,temp=[],[]
        with open("data_temp.csv") as f:
            for line in f.readlines():
                if "," in line and not line.startswith("#"):
                    try:
                        line=line.split(",")
                        tm,tp=float(line[0]),float(line[1])
                        if tp<100:
                            times.append(tm)
                            temp.append(tp)
                    except:
                        print("fail")
        times=np.array(times)
        times=times-times[0]
        times=times/60.0
        
        temp=np.array(temp)
        temp=temp*9/5+32 # now in F
        plt.figure(figsize=(8,8))
        
        plt.grid()
        plt.plot(times,temp,'.',color='#9999FF')
        plt.plot(times,smooth(temp),'k-',alpha=.5,lw=2)
        if len(times)>60*5:
            plt.axvline(times[60*5],color='r',ls='--',lw=3,alpha=.2)
#        if times[-1]>60:
#            plt.axvline(times[60*5+60*60],color='r',ls='--',lw=3,alpha=.2)
        plt.xlabel("experiment duration (minutes)")
        plt.ylabel("temperature (Fahrenheit)")
        plt.title("Temperature")
        plt.margins(0,.1)
        
        plt.tight_layout()
        plt.show()

        print("DONE")
#        break
        time.sleep(5)