import matplotlib.pyplot as plt
import numpy as np

with open('data.csv') as f:
    raw=f.read().split("\n")
ts,cs,ps=[],[],[]
for line in raw:
    try:
        t,c,p=line.split(",")
        t,c,p=float(t),float(c),float(p)
        ts.append(t)
        cs.append(c)
        ps.append(p)
    except:
        print("failed on a line",line)
print("found %d records."%len(ts))
ts,cs,ps=np.array(ts),np.array(cs),np.array(ps)
psi=ps*0.0145038
times=(ts-ts[0])/3600

plt.figure(figsize=(10,10))

ax1=plt.subplot(211)
plt.grid()
plt.plot(times,cs,'b',lw=2,alpha=.6)
plt.ticklabel_format(useOffset=False)
plt.ylabel("temperature (C)")

ax2=plt.subplot(212,sharex=ax1)
plt.grid()
plt.plot(times,ps,'r',lw=2,alpha=.6) 
plt.ticklabel_format(useOffset=False)
plt.ylabel("pressure (mbar)")
plt.xlabel("time (hours)")
plt.margins(0,.1)

plt.show()
print("DONE")