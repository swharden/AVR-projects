import numpy as np
import pylab

if __name__=="__main__":
    with open("capture.txt") as f:
        raw=f.read()
    vals=[]
    for line in raw.split("\n"):
        if len(line)<3:
            continue
        vals.append(int(line.strip()))
    vals=np.array(vals)
    print(vals)
    pylab.plot(vals)
    pylab.show()