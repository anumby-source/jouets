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


#include <LCD5110_Basic.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Servo.h"

LCD5110 myGLCD(4,5,6,8,7);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

#define PIN_PLUS 3 // alimentation Nokia
#define PIN_BL 2 // fond ecran blanc

MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;

int servoPin1 = 11, servoPin2 = 13, servoPin3 = 12;
Servo servo1, servo2, servo3;

// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO

int invertLCD=0;

void setup()
{
        servo1.attach(servoPin1); 
        servo2.attach(servoPin2);
        servo3.attach(servoPin3);
        servo1.write(90); //Init the servo1 angle to 0
        servo2.write(90); //Init the servo2 angle to 0
        servo3.write(90); //Init the servo2 angle to 0
        delay(1000);
        
//#define PIN_PLUS 3 // alimentation Nokia
//#define PIN_BL 2 // fond ecran blanc
pinMode(3, OUTPUT);      
pinMode(2, OUTPUT); 
digitalWrite(3, HIGH);
digitalWrite(2, HIGH);


  myGLCD.InitLCD();
  myGLCD.setContrast(60);
    Serial.begin(115200);
  Wire.begin();

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");


    // supply your own gyro offsets here, scaled for min sensitivity
    accelgyro.setXGyroOffset(220);
    accelgyro.setYGyroOffset(76);
    accelgyro.setZGyroOffset(-85);
    accelgyro.setZAccelOffset(1788); // 1688 factory default for my test chip
    
}

void loop()
{
    unsigned long currentMillis = millis()/1000;
    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);


        // display tab-separated accel/gyro x/y/z values
        Serial.print("a/g:\t");
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.println(gz);
        
  

    myGLCD.clrScr();

  Serial.print("x = ");
  Serial.println(ax);
  Serial.print("y = ");
  Serial.println(ay);
  Serial.print("z = ");
  Serial.println(az);

   myGLCD.setFont(MediumNumbers);
    myGLCD.printNumI(ax /184, RIGHT, 0); // 16 600 / 90
        myGLCD.printNumI(ay /184, RIGHT, 20);
            myGLCD.printNumI(az /184, RIGHT, 35);
               myGLCD.setFont(SmallFont);     myGLCD.print("Ax",LEFT, 0);     myGLCD.print("Ay",LEFT, 20);
 if ( (gz>500) || (gz< -500) ) {digitalWrite(PIN_BL, HIGH);}    else { digitalWrite(PIN_BL, LOW);};
//  if (ay>0) {myGLCD.invert(invertLCD % 2);}    
  Serial.print ("Heading angle = ");
  Serial.print (az);
  Serial.println(" Degree");
 //   myGLCD.setFont(MediumNumbers);
 //   myGLCD.print("Compas",RIGHT, 0);
//    myGLCD.setFont(BigNumbers);
//    myGLCD.printNumI(ay, RIGHT, 24);
   myGLCD.invert(currentMillis % 2);

           servo1.write(map(ay, -16600, 16600, 180, 0)); //Control servo1
            servo2.write(map(ax, -16600, 16600, 180, 0)); //Control servo2


  delay(50);
  

}
