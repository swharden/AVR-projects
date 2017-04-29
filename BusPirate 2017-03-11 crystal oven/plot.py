import matplotlib.pyplot as plt
if __name__=="__main__":
    with open('data.csv') as f:
        raw=f.read().split("\n")
    times,temps,freqs=[],[],[]
    for line in raw:
        if "," in line and not line.startswith("#"):
            a,b,c=line.strip().split(",")
            times.append(float(a)/60)
            temps.append(float(b))
            freqs.append(float(c))
            if len(freqs)>10:
                if abs(freqs[-1]-freqs[-5])>50:
                    freqs[-1]=freqs[-2]
            
    plt.figure(figsize=(10,10))
    ax=plt.subplot(211)
    plt.grid()
    plt.plot(times,temps)
    plt.subplot(212,sharex=ax)
    plt.grid()
    plt.plot(times,freqs)
    
    
    plt.figure(figsize=(10,10))
    plt.plot(temps,freqs)
    plt.show()
    
    print("DONE")