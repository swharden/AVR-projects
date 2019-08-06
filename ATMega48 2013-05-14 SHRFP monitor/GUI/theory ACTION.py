import matplotlib
matplotlib.use('TkAgg') # THIS MAKES IT FAST!
import numpy
import random
import pylab
import scipy.signal

from SwhRecorder2 import *

docs=

LENGTH_0=20
LENGTH_1=40
LENGTH_2=60
LENGTH_FUDGE=3

SAMPLEBY=4

### MAKE PERFECT DATA ###
data=[]
orig=""
for i in range(50):
    data+=[0]*LENGTH_0
    if random.randint(0,1)==0:
        data+=[1]*LENGTH_0
        orig+="0"
    else:
        data+=[1]*LENGTH_1
        orig+="1"
    if i%10==0:
        data+=[0]*LENGTH_0+[1]*LENGTH_2
        orig+="b"
        

### ADD SOME NOISE ### (simulates excessively weak signal)
#for i in range(len(data)-3):
#    if random.randint(1,40)==10:
#        for j in range(random.randint(1,3)):
#            data[i+j]=abs(data[i+j]-1)
       
### DROP SOME VALUES (simulating inaccurate timing) ###
i=0
while i<len(data):
    if random.randint(0,20)==0:
        data.pop(i)
    i+=1    
        
### RESAMPLE    
    
### GOTTA BE NUMPY WHEN GIVEN FROM SOUND CARD ###
# SAMPLE DOWN 
data=scipy.signal.resample(data,len(data)/SAMPLEBY)
LENGTH_0=20/SAMPLEBY
LENGTH_1=40/SAMPLEBY
LENGTH_2=60/SAMPLEBY
LENGTH_FUDGE=10/SAMPLEBY
if LENGTH_FUDGE==0: LENGTH_FUDGE+=1

print "FINAL SAMPLE LENGTHS:",LENGTH_0,LENGTH_1,LENGTH_2,LENGTH_FUDGE

### FIND DISTANCE BETWEEN ALL LOW POINTS ###
data=numpy.where(data<.5)[0]
data=numpy.ediff1d(data)

### START DETECTING PULSES ###

pulses=''
for i in range(len(data)):
    pulseLength=data[i]
    
    if pulseLength<(LENGTH_0-LENGTH_FUDGE):
        #consecutive data point, not done with bit
        continue
    
    elif (LENGTH_0-LENGTH_FUDGE)<pulseLength<(LENGTH_0+LENGTH_FUDGE):
        pulses+='0'
    elif (LENGTH_1-LENGTH_FUDGE)<pulseLength<(LENGTH_1+LENGTH_FUDGE):
        pulses+='1'
    elif (LENGTH_2-LENGTH_FUDGE)<pulseLength<(LENGTH_2+LENGTH_FUDGE):
        pulses+='b'
        
    else:
        pulses+='?'

print orig
print pulses