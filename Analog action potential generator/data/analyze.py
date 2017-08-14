import numpy as np
import matplotlib.pyplot as plt
Ys = np.memmap("recording.wav", dtype='h', mode='r')[1000:40000]
Ys = np.array(Ys)/max(Ys)*150-70
Xs = np.arange(len(Ys))/44100*1000
plt.figure(figsize=(6,3))
plt.grid(alpha=.5,ls=':')
plt.plot(Xs,Ys)
plt.margins(0,.1)
plt.title("Action Potential Circuit Output")
plt.ylabel("potential (mV)")
plt.xlabel("time (ms)")
plt.tight_layout()
plt.savefig("graph.png")
#plt.show()