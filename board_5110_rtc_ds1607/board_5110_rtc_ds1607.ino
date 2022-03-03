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
#include "RTClib.h"

RTC_DS1307 rtc;
/*#include "compass.h"
#include <SFE_BMP180.h>

SFE_BMP180 pressure;
#define ALTITUDE 55.0 // Altitude of Bagneux
*/

LCD5110 myGLCD(4,5,6,8,7);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

int invertLCD=0;

void setup()
{

   Serial.begin(9600);
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
pinMode(PIN_PLUS, OUTPUT);      // sets the digital pin as output
pinMode(PIN_BL, OUTPUT); 
digitalWrite(PIN_PLUS, HIGH);
digitalWrite(PIN_BL, HIGH);
pinMode(A6, INPUT_PULLUP);      // alimentation H5883l 
pinMode(A7, INPUT_PULLUP);
//digitalWrite(A7, LOW);
//digitalWrite(A6, LOW);

pinMode(A0, OUTPUT);      // alimentation H5883l 
pinMode(A1, OUTPUT);
digitalWrite(A0, HIGH);
digitalWrite(A1, LOW);


  myGLCD.InitLCD();
  myGLCD.setContrast(60);
    Serial.begin(9600);
  Wire.begin();
  /*
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
  */
}

void loop()
{

   DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
//  char status;
//  double T,P,p0,a;
    myGLCD.clrScr();
  
  invertLCD++;

    myGLCD.setFont(SmallFont);
      myGLCD.print("Min",LEFT, 0);
   myGLCD.setFont(MediumNumbers);
    myGLCD.printNumF( float (100 * now.hour()+ now.minute())/100, 2, RIGHT, 0);

     myGLCD.setFont(SmallFont);
        myGLCD.print("Sec",LEFT, 42);
    myGLCD.setFont(BigNumbers);

    myGLCD.printNumI(now.second(), RIGHT, 24);
   myGLCD.invert(invertLCD % 2);
        delay(1000);

/*
  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.print(" deg C, ");

      
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");

          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");

          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          a = pressure.altitude(P,p0);
          Serial.print("computed altitude: ");
          Serial.print(a,0);
          Serial.print(" meters, ");
  
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");


 //   myGLCD.setFont(MediumNumbers);
 //   myGLCD.print("BMP180",RIGHT, 0);
  myGLCD.setFont(SmallFont);
      myGLCD.print("Temp",LEFT, 0);
   myGLCD.setFont(MediumNumbers);
    myGLCD.printNumF(T, 2, RIGHT, 0);

     myGLCD.setFont(SmallFont);
        myGLCD.print("Pression",LEFT, 42);
    myGLCD.setFont(BigNumbers);

    myGLCD.printNumI(p0, RIGHT, 24);
   myGLCD.invert(invertLCD % 2);
        delay(1000);

        */
  
  

}
