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

tof.start()
f1=500
print("started")
f2=(tof.read())
while True:
    f2=((tof.read())+7*f1)//8
    sound.freq(f2)
    time.sleep_ms(20)
    f1=f2
    
