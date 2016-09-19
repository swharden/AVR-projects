import numpy as np
import pylab

def getData(fname='captureWarmup.txt'):
    with open(fname) as f:
        raw=f.read().replace("\n\n","\n").split("\n")
    skipped=0
    Vs,Is=[],[]
    for i,line in enumerate(raw):
        if i<3:
            continue
        if not raw[i-2]==raw[i-1]==raw[i]:
            continue # ensure 3 in a row are valid
        try:
            line=line.split(",")
            line=[float(line[0]),float(line[1])]
            Vs.append(line[0])
            Is.append(line[1])
        except:
            skipped+=1
    print("skipped %d of %d lines (%.02f%%)"%(skipped,i,100*skipped/i))
    return np.arange(len(Vs)),np.array(Vs),np.array(Is)

if __name__=="__main__":
    Xs,Vs,Is=getData()
    
    pylab.figure()
    ax=pylab.subplot(211)
    pylab.plot(Vs,'r-')
    pylab.grid()
    pylab.ylabel("potential (V)")
    pylab.subplot(212,sharex=ax)
    pylab.plot(Is,'b-')
    pylab.grid()
    pylab.ylabel("current (mA)")
    pylab.xlabel("time (# readings)")
    pylab.tight_layout()
    pylab.show()