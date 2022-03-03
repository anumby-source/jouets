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
#include "compass.h"

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
digitalWrite(PIN_BL, HIGH);
pinMode(A6, OUTPUT);      // alimentation H5883l 
pinMode(A7, OUTPUT);
digitalWrite(A7, HIGH);
digitalWrite(A6, HIGH);

pinMode(A0, OUTPUT);      // alimentation H5883l 
pinMode(A1, OUTPUT);
digitalWrite(A0, HIGH);
digitalWrite(A1, LOW);


  myGLCD.InitLCD();
  myGLCD.setContrast(55);
    Serial.begin(9600);
  Wire.begin();
  compass_x_offset = 122.17;
  compass_y_offset = 230.08;
  compass_z_offset = 389.85;
  compass_x_gainError = 1.12;
  compass_y_gainError = 1.13;
  compass_z_gainError = 1.03;
  compass_init(2);
}

void loop()
{

    myGLCD.clrScr();
  
  invertLCD++;
   compass_scalled_reading();
  Serial.print("x = ");
  Serial.println(compass_x_scalled);
  Serial.print("y = ");
  Serial.println(compass_y_scalled);
  Serial.print("z = ");
  Serial.println(compass_z_scalled);

   myGLCD.setFont(MediumNumbers);
    myGLCD.printNumF(compass_x_scalled, 2, RIGHT, 0);
 if (compass_x_scalled>0) {digitalWrite(PIN_BL, HIGH);}    else { digitalWrite(PIN_BL, LOW);};
  if (compass_y_scalled>0) {myGLCD.invert(invertLCD % 2);}    
  compass_heading();
  Serial.print ("Heading angle = ");
  Serial.print (bearing);
  Serial.println(" Degree");
 //   myGLCD.setFont(MediumNumbers);
 //   myGLCD.print("Compas",RIGHT, 0);
    myGLCD.setFont(BigNumbers);
    myGLCD.printNumI(bearing-180, RIGHT, 24);
   myGLCD.invert(invertLCD % 2);
        delay(1000);
  
  

}
