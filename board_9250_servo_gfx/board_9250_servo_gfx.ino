////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib-Arduino
//
//  Copyright (c) 2014-2015, richards-tech
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <Wire.h>
#include "I2Cdev.h"
#include "RTIMUSettings.h"
#include "RTIMU.h"
#include "RTFusionRTQF.h"
#include "CalLib.h"
#include <EEPROM.h>
#include "Servo.h"
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define PIN_PLUS 3 // alimentation Nokia
#define PIN_BL 2 // fond ecran blanc
#define SCL 3
#define SDA 4
#define OLED_RESET 0
Adafruit_PCD8544 display = Adafruit_PCD8544(4, 5, 6, 7, 8);

#define TFTX  84  // Maximale Höhe der Balken in Pixel
#define TFTY  48  // Maximale Höhe der Balken in Pixel  
#define cx 60
#define cy 22
#define cr 20
int heading_degrees_old = 0; // angle de l'aiguille

Servo servo1, servo2, servo3;

RTIMU *imu;                                           // the IMU object
RTFusionRTQF fusion;                                  // the fusion object
RTIMUSettings settings;                               // the settings object

//  DISPLAY_INTERVAL sets the rate at which results are displayed

#define DISPLAY_INTERVAL  150                         // interval between pose displays

//  SERIAL_PORT_SPEED defines the speed to use for the debug serial port

#define  SERIAL_PORT_SPEED  115200

unsigned long lastDisplay;
unsigned long lastRate;
int sampleCount;

void setup()
{
  int errcode;

  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(3, HIGH);
  digitalWrite(2, HIGH);

  //  display.setTextColor(WHITE);


  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(55);
  /*
    display.display(); // show splashscreen
    delay(2000);
    display.clearDisplay();   // clears the screen and buffer

      testfillroundrect();
    delay(2000);
    display.clearDisplay();
  */

  // text display tests
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
 // display.println("Maquette");
   display.println("MPU9250");
    display.println("Arnaud");
  display.display();
  delay(2000);


  servo1.attach(12); // yaw
  servo2.attach(11);
  servo3.attach(13);

  servo1.write(0); //Init the servo1 angle to 0
  servo2.write(0); //Init the servo2 angle to 0
  servo3.write(0);

  delay(500);

  Serial.begin(SERIAL_PORT_SPEED);
  Wire.begin();
  imu = RTIMU::createIMU(&settings);                        // create the imu object

  Serial.print("ArduinoIMU starting using device "); Serial.println(imu->IMUName());
  if ((errcode = imu->IMUInit()) < 0) {
    Serial.print("Failed to init IMU: "); Serial.println(errcode);
  }

  if (imu->getCalibrationValid())
    Serial.println("Using compass calibration");
  else
    Serial.println("No valid compass calibration data");

  lastDisplay = lastRate = millis();
  sampleCount = 0;

  // Slerp power controls the fusion and can be between 0 and 1
  // 0 means that only gyros are used, 1 means that only accels/compass are used
  // In-between gives the fusion mix.

  fusion.setSlerpPower(0.02);

  // use of sensors in the fusion algorithm can be controlled here
  // change any of these to false to disable that sensor

  fusion.setGyroEnable(true);
  fusion.setAccelEnable(true);
  fusion.setCompassEnable(true);
}

void loop()
{
  unsigned long now = millis();
  unsigned long delta;
  int loopCount = 1;
  RTVector3 vec;
  int p, y, r, mx, my, mz; // yaw pitch roll

  while (imu->IMURead()) {                                // get the latest data if ready yet
    // this flushes remaining data in case we are falling behind
    if (++loopCount >= 10)
      continue;
    fusion.newIMUData(imu->getGyro(), imu->getAccel(), imu->getCompass(), imu->getTimestamp());
    sampleCount++;
    if ((delta = now - lastRate) >= 1000) {
      Serial.print("Sample rate: "); Serial.print(sampleCount);
      if (imu->IMUGyroBiasValid())
        Serial.println(", gyro bias valid");
      else
        Serial.println(", calculating gyro bias");

      sampleCount = 0;
      lastRate = now;
    }
    if ((now - lastDisplay) >= DISPLAY_INTERVAL) {
      lastDisplay = now;
      //          RTMath::display("Gyro:", (RTVector3&)imu->getGyro());                // gyro data
      //          RTMath::display("Accel:", (RTVector3&)imu->getAccel());              // accel data
      RTMath::display("Mag:", (RTVector3&)imu->getCompass());              // compass data
      RTMath::displayRollPitchYaw("Pose:", (RTVector3&)fusion.getFusionPose()); // fused output

      vec = fusion.getFusionPose();
      r = vec.x() * RTMATH_RAD_TO_DEGREE;
      p = vec.y() * RTMATH_RAD_TO_DEGREE;
      y = vec.z() * RTMATH_RAD_TO_DEGREE;
      servo1.write(map(y, -180, 180, 0, 180)); //Control servo1
      servo2.write(map(p, -90, 90, 0, 180)); //Control servo2
      servo3.write(map(r, -90, 90, 180, 0)); //Control servo

      vec = imu->getCompass();
      mx = vec.x();
      my = vec.y();
      mz = vec.z();
      pinMode(3, OUTPUT);
      pinMode(2, OUTPUT);
      digitalWrite(3, HIGH);
      digitalWrite(2, HIGH);

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE, BLACK); // 'inverted' text

      display.setCursor(0, 0);
      display.println("YPR");
      display.setTextColor(BLACK);
      display.setTextSize(1);
      display.println(y);

      display.println(p);
      display.println(r);
      display.print(mx); display.print(":"); display.print(my); display.print(":"); display.print(mz);
     aiguille(y);// tracé
      petite_aiguille(  180 * atan2 ( mx,my) / M_PI);
      hbar(p);
  //    vbar(r);
                      display.display();
                      Serial.println();
    }
  }
}

void testfillroundrect(void) {
  uint8_t color = BLACK;
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}
void aiguille(int h) { // aiguille en degré
  //  display.drawCircle(cx,cy,cr+1,BLACK);
  display.drawLine((int)cx, (int)cy, (int)(cx + cr * cos(M_PI / 180 * h)), (int)(cy - cr * sin(M_PI / 180 * h)), BLACK);
}
void petite_aiguille(int h) { // aiguille en degré
  //  display.drawCircle(cx,cy,cr+1,BLACK);
  display.drawLine((int)cx, (int)cy, (int)(cx + (cr * cos(M_PI / 180 * h)) / 2), (int)(cy - (cr * sin(M_PI / 180 * h) )/ 2), BLACK);
}
void hbar (int aa){ // angle de -90 a +90 barre horizontale 
        int bsize = (TFTY * (aa + 90) )/180;
  //      display.drawLine(0, 0, 0, bsize, BLACK);
        display.drawLine(TFTX-1, 0, TFTX-1, bsize, BLACK);  
    }
  
void vbar (int aa){ // angle de -90 a +90 barre verticale 
          int bsize = (TFTX * (aa + 90) )/180;
 //      display.drawLine(0,0 ,bsize , 0, BLACK);
       display.drawLine(0,TFTY-1 ,bsize , TFTY-1, BLACK);

    }
