import sys,os
import time
from machine import Pin,PWM
from machine import SoftI2C
import VL53L0X

i2c = SoftI2C(scl=Pin(21),sda=Pin(22))

# Create a VL53L0X object
tof = VL53L0X.VL53L0X(i2c)
sound=PWM(Pin(18), freq=20, duty=512)

tof.set_Vcsel_pulse_period(tof.vcsel_period_type[0], 18)

tof.set_Vcsel_pulse_period(tof.vcsel_period_type[1], 14)

# def TimeOut(t):
#     start=ticks_ms()
#     def compare():
#         return int(ticks_ms()-start) >= t
#     return compare

tof.start()
f1=tof.read()
print("started")

while True:
    f2=(tof.read())
    df=(f2-f1)
    if df != 0:
        s= df//10 if df >= 50 else  df//abs(df)
    if s != 0:
        for f in range(f1,f2,s):
            sound.freq(f)
            time.sleep_ms(10)
    f1=f2
    f2=(tof.read())
    # f=(f1+f2)//2 # oder
    # f=f1+f2 # oder
    # f=f2+os.urandom(1)[0]*2 #oder
    # f=os.urandom(1)[0]*2
    # sound.freq(f)
    # f1=f2 



    #q = tof.set_signal_rate_limit(0.1)
    #
    # time.sleep(0.1)
