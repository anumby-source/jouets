/* Get tilt angles on X and Y, and rotation angle on Z
 * Angles are given in degrees
 * 
 * License: MIT
 * 
 * Ajout d'un servomoteur sur pin 3
 */

#include "Wire.h"
#include <MPU6050_light.h>
#include "Servo.h"

MPU6050 mpu(Wire);
unsigned long timer = 0;
Servo servo1, servo2, servo3;

#define PIN_PLUS 2
#define PIN_SERVO 3 // servomoteur 

void setup() {
  Serial.begin(115200);
     servo1.attach(PIN_SERVO); // yaw
  pinMode(PIN_PLUS, OUTPUT);      // sets the digital pin as output
digitalWrite(PIN_PLUS, HIGH);
  Wire.begin();
  
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
}

void loop() {
  mpu.update();
  
  if((millis()-timer)>10){ // print data every 10ms
	Serial.print("X : ");
	Serial.print(mpu.getAngleX());
	Serial.print("\tY : ");
	Serial.print(mpu.getAngleY());
	Serial.print("\tZ : ");
	Serial.println(mpu.getAngleZ());

          servo1.write(map(mpu.getAngleY(), -90, 90, 0, 180)); //Control servo1
          
	timer = millis();  
  }
}
