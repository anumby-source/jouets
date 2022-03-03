// LCD5110_NumberFonts 
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a demo of the included number-fonts,
// and how to use them.
//
// This program requires a Nokia 5110 LCD module
// and Arduino 1.5.2 (or higher).
//
// It is assumed that the LCD module is connected to
// the following pins.
//      SCK  - Pin 8 >> 4
//      MOSI - Pin 9 >> 5
//      DC   - Pin 10 >> 6
//      RST  - Pin 11 >> 8
//      CS   - Pin 12 >> 7
//
#define PIN_PLUS 3
#define PIN_BL 2

#include <LCD5110_Basic.h>
#include <Wire.h>
#include <APDS9930.h>

APDS9930 apds = APDS9930();
uint16_t proximity_data = 0;
float ambient_light = 0; // can also be an unsigned long
uint16_t ch0 = 0;
uint16_t ch1 = 1;

LCD5110 myGLCD(4,5,6,8,7);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

int invertLCD=0;

void setup()
{
  
pinMode(PIN_PLUS, OUTPUT);      // sets the digital pin as output
pinMode(PIN_BL, OUTPUT); 
digitalWrite(PIN_PLUS, HIGH);
digitalWrite(PIN_BL, LOW);
  myGLCD.InitLCD();
  myGLCD.setContrast(60);
    Serial.begin(9600);
  Wire.begin();
    Serial.println(F("---------------------------"));
  Serial.println(F("APDS-9930 - ProximitySensor"));
  Serial.println(F("---------------------------"));
  // Initialize APDS-9930 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9930 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9930 init!"));
  }
  
  // // Adjust the Proximity sensor gain
  // if ( !apds.setProximityGain(PGAIN_2X) ) {
  //   Serial.println(F("Something went wrong trying to set PGAIN"));
  // }
  

}

void loop()
{
  char status;
    myGLCD.clrScr();
  invertLCD++;
 
  // Start running the APDS-9930 proximity sensor (no interrupts)
  if ( apds.enableProximitySensor(false) ) {
    Serial.println(F("Proximity sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during sensor init!"));
  }
     delay(200);
  // Read the proximity value
  if ( !apds.readProximity(proximity_data) ) {
    Serial.println("Error reading proximity value");
  } else {
    Serial.print("Proximity: ");
    Serial.println(proximity_data);
  }
     delay(200);
     
   if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during light sensor init!"));
  }
  
    // Read the light levels (ambient, red, green, blue)
  if (  !apds.readAmbientLightLux(ambient_light) ||
        !apds.readCh0Light(ch0) || 
        !apds.readCh1Light(ch1) ) {
    Serial.println(F("Error reading light values"));
  } else {
    Serial.print(F("Ambient: "));
    Serial.print(ambient_light);
    Serial.print(F("  Ch0: "));
    Serial.print(ch0);
    Serial.print(F("  Ch1: "));
    Serial.println(ch1);
  }


  
 //   myGLCD.setFont(MediumNumbers);
 //   myGLCD.print("BMP180",RIGHT, 0);
  myGLCD.setFont(SmallFont);
      myGLCD.print("Proxi",LEFT, 0);
   myGLCD.setFont(MediumNumbers);
    myGLCD.printNumI(proximity_data, RIGHT, 0);

     myGLCD.setFont(SmallFont);
        myGLCD.print("Lux",LEFT, 42);
    myGLCD.setFont(BigNumbers);

    myGLCD.printNumF(ambient_light,0, RIGHT, 24);
   myGLCD.invert(invertLCD % 2);
      
    delay(500);
  

}
