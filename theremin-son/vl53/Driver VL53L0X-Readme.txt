Driver VL53L0X

1) Initialisation

>>> from machine import Pin, I2C
>>> from VL53L0X import *
>>> i2c = I2C(0, sda=Pin(0), scl=Pin(1))
>>> v =  VL53L0X(i2c)
>>> v.init()

2) Mesures

Mode simple:

>>> v.readRangeSingleMillimeters()   # renvoie 8190 si overflow

Mode continu:

>>> v.startContinuous(50)   # mesures timées: 1 mesure/50 ms
ou
>>> v.startContinuous(0)    # non timées, aucun delay entre 2 mesures
puis
>>> v.readRangeContinuousMillimeters()   # renvoie 8190 si overflow
et
>>> v.stopContinuous()      # pour repasser en mesure simple

3) Range max ~ 1200 mm. Pour aller au dela:

>>> v.setLongRange()
et pour revenir en range par defaut
>>> v.setDefaultRange()

4) timing_budget = temps alloué à une mesure (en us), min = 20000 us

5) Validation de la dernière mesure:

>>> v.getRangeStatus()
renvoie	
0:"range valid", 2:"Signal Fail", 3 : "Min range Fail", 
4 : "Phase Fail", 5 : "Hardware Fail"
ou
>>> v.getRangeStatusString()

6) Remarques:
- en mode mesure simple, pour déclencher une mesure, on met à 1 le bit 0
  de SYSRANGE_START. Le bit repasse à 0 quand la mesure démarre (voir la
  methode readRangeSingleMillimeters())
- pour mettre plusieurs capteurs sur le même port I2C, il faut changer
  l'adresse I2C (voir methode setAdress()). La nouvelle adresse est conservée
  jusqu'au prochain shutdown.
- Interrupt : Pin GPIO01 -> passe à 0 lorsqu'une mesure est disponible et 
  repasse à 1 après reset ( writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01) )
  la lecture d'une mesure (readRangeSingleMillimeters ou readRangeContinuousMillimeters) 
  reset l'interruption
- Pin XSHUT : état 1 (PULL-UP) en fonctionnement normal. Le passage à 0
  met le composant en mode Shutdown.