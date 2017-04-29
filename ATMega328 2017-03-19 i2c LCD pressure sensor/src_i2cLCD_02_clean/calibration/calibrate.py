"""
Use program memory of a MS5611 temperature / pressure sensor to create
a calculation to approximate pressure (in PSI) easily with simple math.
You have to read the program memory at least once to get the 6 calibration
values. I imagine they're extremely similar from chip to chip.
datasheet: http://www.hpinfotech.ro/MS5611-01BA03.pdf

"""
import numpy as np
import matplotlib.pyplot as plt

# values looked up from program memory
C1=44433
C2=47103
C3=27811
C4=25673
C5=32440
C6=28302

# generate a range of possible ADC readings
X=np.linspace(0,2**24,100)
D1=np.copy(X) # digital pressure value
D2=8610000 # 22 C (typical temperature)

# calculate temperature
dT=D2-C5*(2**8) # difference between actual and reference temperature
Tc=(2000+dT*C6/(2**23))/100 # temperature (C) with 0.01C resolution

pOff=C2*(2**16)+(C4*dT)/(2**7) # offset pressure
sens=C1*(2**15)+(C3*dT)/(2**8) # pressure sensitivity at pressure
Pmb=((D1*sens/(2**21)-pOff)/(2**15))/100 # pressure (mb) with 0.01 resolution
Ppsi=Pmb*0.0145038 # pressure (psi)

plt.figure()
plt.plot(X,Ppsi)
plt.grid()
slope=(Ppsi[-1]-Ppsi[0])/X[-1]
plt.title("PSI @ 22C = ADC/%.03f - %.03f"%(1/slope,abs(Ppsi[0])))
plt.ylabel("pressure (PSI)")
plt.xlabel("24-bit ADC reading")
plt.show()