// King Tide NE MARCHE PAS AVEC ECRAN

#include <Wire.h>
#include "I2Cdev.h"
#include "RTIMUSettings.h"
#include "RTIMU.h"
#include "RTFusionRTQF.h" 
#include "CalLib.h"
#include <EEPROM.h>

#include "Servo.h"
#include "Adafruit_ST7735.h" // Adafruit ST7735-Bibliothek
#include "Adafruit_GFX.h"    // Adafruit Grafik-Bibliothek

RTIMU *imu;                                           // the IMU object
RTFusionRTQF fusion;                                  // the fusion object
RTIMUSettings settings;                               // the settings object

//  DISPLAY_INTERVAL sets the rate at which results are displayed

#define DISPLAY_INTERVAL  500                         // interval between pose displays

//  SERIAL_PORT_SPEED defines the speed to use for the debug serial port

#define  SERIAL_PORT_SPEED  115200

unsigned long lastDisplay;
unsigned long lastRate;
int sampleCount;

#define RST    3  // you can also connect this to the Arduino reset
#define CS 4
#define RS   5
#define SDA   6
#define SCL   7
#define PIN_PLUS 8
#define PIN_BL 9
#define PIN_MOINS 10

// Color definitions
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF

// Adafruit_ST7735(int8_t cs, int8_t rs, int8_t sid, int8_t sclk, int8_t rst) 
Adafruit_ST7735 tft = Adafruit_ST7735(CS, RS, SDA, SCL, 0);  // Display-Bibliothek Setup

int old_h=0;// barre horirontale verticale angle en degré
int old_v=0;
int old_a=0;

// cercle 
#define cx 64
#define cy 80
#define cr 45

Servo servo1, servo2, servo3;

// ================================================================
// ===                    INITIAL SETUP                        ===
// ================================================================

void setup() {

   int errcode;
  
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
    
    fusion.setGyroEnable(true);
    fusion.setAccelEnable(true);
    fusion.setCompassEnable(true);

pinMode(PIN_PLUS, OUTPUT);      // sets the digital pin as output
pinMode(PIN_BL, OUTPUT); 
digitalWrite(PIN_PLUS, HIGH);
digitalWrite(PIN_BL, HIGH);

/*

       servo1.attach(12); // yaw
        servo2.attach(11);
        servo3.attach(13);

       servo1.write(0); //Init the servo1 angle to 0
        servo2.write(0); //Init the servo2 angle to 0
        servo3.write(0); //In if(aa>90) { aaa-= 180; }
 if(aa < -90) { aa+= 180; }it the servo2 angle to 0

                
     delay(500);
 */ 
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(BLACK);
    tft.setRotation(0);
      tft.drawCircle(cx,cy,cr+1,BLUE);
  tft.drawCircle(cx,cy,cr-1,RED);

    set_text(14,3,"MPU9250",ST7735_BLUE,2);  
    set_text(14,140,"maquette Arnaud",ST7735_GREEN,1);
  
}
// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    unsigned long now = millis();
    unsigned long delta;
    int loopCount = 1;
    RTVector3 vec;
    int x,y,z;
           
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
//          RTMath::display("Mag:", (RTVector3&)imu->getCompass());              // compass data
            RTMath::displayRollPitchYaw("Pose:", (RTVector3&)fusion.getFusionPose()); // fused output
           Serial.println();
           vec = fusion.getFusionPose();
           x=vec.x() * RTMATH_RAD_TO_DEGREE;
           y=vec.y() * RTMATH_RAD_TO_DEGREE;
           z=vec.z() * RTMATH_RAD_TO_DEGREE;
  
           display_graphic (z);
           hbar( y ) ;// barre horizontale
          vbar( x ) ;// barre verticale

//          servo1.write(map(ypr[0] * 180/M_PI, -180, 180, 0, 180)); //Control servo1
//            servo2.write(map(ypr[1] * 180/M_PI, -90, 90, 0, 180)); //Control servo2
//            servo3.write(map(ypr[2] * 180/M_PI, -90, 90, 180,0)); //Control servo
       }
    }
   
}

void hbar (int aa){ // angle de -90 a +90 barre horizontale


      uint16_t tfth = 160 ; // Maximale Höhe der Balken in Pixel
      uint32_t bsize;           // Balkenhöhe in Pixel
    if(aa>90) { aa -= 180; }
    if(aa < -90) { aa += 180; }
      bsize = (tfth * (aa + 90) )/180;
      if (old_h > bsize) {
        tft.drawLine(0,bsize, 0, old_h, BLACK);// efface de old a bsize
      } else {
        tft.drawLine(0, old_h, 0, bsize, GREEN);
      }
      old_h = bsize;
    }
  
void vbar (int aa){ // angle de -90 a +90 barre horizontale

      uint16_t tfth = 128 ; // Maximale Höhe der Balken in Pixel
      uint32_t bsize;           // Balkenhöhe in Pixel
    if(aa>90) { aa -= 180; }
    if(aa < -90) { aa += 180; }
      bsize = (tfth * (aa + 90) )/180;
      if (old_v > bsize) {
        tft.drawLine(bsize,0 , old_v, 0, BLACK);
      } else {
       tft.drawLine(old_v,0 ,bsize , 0, CYAN);
      }
      old_v = bsize;
    }

void display_graphic(int h){ // angle de -90 a +90 barre horizontale
  
  if( old_a != h) { 

  tft.drawLine((int)cx,(int)cy,(int)(cx+(cr-4)*cos(M_PI/180*old_a)),(int)(cy-(cr-4)*sin(M_PI/180*old_a)),ST7735_BLACK);
  tft.drawLine((int)cx,(int)cy,(int)(cx+(cr-4)*cos(M_PI/180*h)),(int)(cy-(cr-4)*sin(M_PI/180*h)),WHITE);
      old_a = h;
}
}
void set_text(int x,int y,String text,uint16_t color,byte size){
  
  tft.setTextSize(size);
  tft.setCursor(x,y);
  tft.setTextColor(color);
  tft.print(text);
}
