from machine import Pin, I2C, PWM
from VL53L0X import *
from time import sleep_ms

sound=PWM(Pin(3), freq=20, duty=512)

i2c = I2C(0, sda=Pin(0), scl=Pin(1))
v =  VL53L0X(i2c)
v.init()

# v.setLongRange()
v.setDefaultRange()
print("started")
f1=(v.readRangeSingleMillimeters())
print(f1)
# mode lecture simple
print('mode single')

print('mode single')
while True:
    try:
        f1 = (v.readRangeSingleMillimeters())
        print( f1)
        sound.freq(f1)
        sleep_ms(20)
    except:
        break


while True:
    try:
        #f2=(v.readRangeSingleMillimeters()+7*f1)/8
        f1=(v.readRangeSingleMillimeters())
        print(f1)
        #sound.freq(f1)
        time.sleep_ms(20)

        
    except:
        break
