/* Affichage des données de champ magnétique
 * 
 * 
 */

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
#define adresse 0x1E // Adresse i2c du capteur HMC5883

// valeurs requises pour calibrer nos mesures (spécifique à votre module):
const int xmin = -564;
const int xmax = -62;
const int ymin = -277;
const int ymax = 254;

// Pour pointer vers le nord géographique plutôt que vers
// le nord magnétique, on additionne la déclinaison, qui dépend
// de l'endroit où vous êtes:
// http://www.magnetic-declination.com/

float declinaison = 0.22;  // en radians


LCD5110 myGLCD(4,5,6,8,7);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

int invertLCD=0;
  int xbrut, ybrut, zbrut, xcalibree, ycalibree, zcalibree;
  
// --------------- lecture x y z ---------------
void compass(){
  
  // On demande les informations au capteur HMC5883
  Wire.beginTransmission(adresse);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();

  //Lecture des composantes du champ magnétique
  Wire.requestFrom(adresse, 6);
  if (6 <= Wire.available()) {
    xbrut = Wire.read() << 8; //X msb
    xbrut |= Wire.read(); //X lsb
    zbrut = Wire.read() << 8; //Z msb
    zbrut |= Wire.read(); //Z lsb
    ybrut = Wire.read() << 8; //Y msb
    ybrut |= Wire.read(); //Y lsb
  }


}
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
  myGLCD.setFont(SmallFont);
   myGLCD.print("Calibration",RIGHT, 0);
      myGLCD.print("mx",RIGHT, 10);
         myGLCD.print("my",RIGHT, 20);
            myGLCD.print("mz",RIGHT, 30);
   delay(2000);
    Serial.begin(115200);
  Wire.begin();
 compass();
   xcalibree = xbrut ;
  ycalibree = ybrut;
  zcalibree = zbrut;


}

void loop()
{

    myGLCD.clrScr();
  invertLCD++;

 compass();
  xbrut -= xcalibree  ;
  ybrut -= ycalibree  ;
  zbrut -= zcalibree  ;


  // Calcul de l'angle entre le nord magnétique et l'axe x du capteur
  float orientationNord = atan2(ycalibree, xcalibree);

  // Pour pointer vers le nord géographique plutôt que vers
  // le nord magnétique, on additionne la déclinaison

  orientationNord += declinaison;
  orientationNord = (orientationNord * 180) / 3.14;

  Serial.println(orientationNord); 

   myGLCD.setFont(MediumNumbers);
    myGLCD.printNumF(xbrut, 0, RIGHT, 0);
        myGLCD.printNumF(ybrut, 0, RIGHT, 20);
            myGLCD.printNumF(zbrut, 0, RIGHT, 35);
            
 if (xcalibree>0) {digitalWrite(PIN_BL, HIGH);}    else { digitalWrite(PIN_BL, LOW);};
  if (ycalibree>0) {myGLCD.invert(invertLCD % 2);}    

  Serial.print ("Heading angle = ");
  Serial.print (orientationNord);
  Serial.println(" Degree");

    myGLCD.setFont(BigNumbers);
//    myGLCD.printNumI(orientationNord-180, RIGHT, 24);
   myGLCD.invert(invertLCD % 2);
        delay(1000);
  
  

}
