## Interact with LM75A (I2C temperature sensor) with Bus Pirate from Python
For details, see the website: http://www.swharden.com/wp/2017-02-04-logging-i2c-data-with-bus-pirate-and-python/

### Notes for converting ADC measurements in Python:
```python
def data_LM75A(data,F=True):
    """convert data array into C or F temperature."""
    for i,val in enumerate(data):
        if val < 2**15: data[i] = val/2**8
        else: data[i] =  (val-2**16)/2**8
    if F:
        data=data*9/5+32
    return data

def data_ntc10k(data,F=True):
    """convert 10k/10kNTC divider voltage to C or F temperature."""
    data=(10000*data)/(5-data)
    A,B,C = 1.12924E-03,2.34108E-04,0.87755E-07
    for i,v in enumerate(data):
        data[i]=1/(A+B*np.math.log(v)+C*np.math.log(v)**3) # kelvin
    data=data-273.15 # kelvin to C
    if F:
        data=data*9/5+32
    return data
    
def data_LM35(data,F=True):
    """convert LM35 voltage to C or F temperature."""
    data=100*data # now in C
    if F:
        data=data*9/5+32
    return data
```
