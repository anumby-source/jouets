


#include "Adafruit_GFX.h"    // Adafruit Grafik-Bibliothek
#include "Adafruit_ST7735.h" // Adafruit ST7735-Bibliothek
#include <SPI.h>


#define RST    5  // you can also connect this to the Arduino reset
#define CS 6
#define RS   7
#define SDA   8
#define SCL   9
#define PIN_PLUS 10
#define PIN_BL 11
#define PIN_MOINS 12
// Adafruit_ST7735(int8_t cs, int8_t rs, int8_t sid, int8_t sclk, int8_t rst) 
Adafruit_ST7735 tft = Adafruit_ST7735(CS, RS, SDA, SCL, RST);  // Display-Bibliothek Setup

//HMC5883L compass; 
int error = 0;




// Deklination (siehe Text) hier fuer Berlin 3" 19' = 0.0578 rad Paris 0°22
   float heading=0;
float declinationAngle=0.006;

float heading_degrees=0;
float heading_degrees_old=360;

float heading_degrees_correction=-90;
float heading_correction=heading_degrees_correction*M_PI/180;


// Ausgabe TFT-Display

#define cx 64
#define cy 80
#define cr 45
#define cb 5

void display_show(){

  tft.fillScreen(ST7735_BLACK); 
  
  set_text(14,3,"Compas HMC5883l",ST7735_BLUE,1);  
  set_text(14,149,"blog.simtronyx.de",ST7735_GREEN,1);
  
  set_text(59,17,"N",ST7735_WHITE,2);
  set_text(59,130,"S",ST7735_WHITE,2);
  set_text(4,74,"W",ST7735_WHITE,2);
  set_text(114,74,"O",ST7735_WHITE,2);
  
  tft.drawCircle(cx,cy,cr+1,rgb565(128,128,128));
  tft.drawCircle(cx,cy,cr-1,rgb565(128,128,128));
  tft.drawCircle(cx,cy,cr,ST7735_WHITE);
}

void display_graphic(){
  
  int m=0;
  int h=heading_degrees_old+90;
  tft.drawLine((int)cx,(int)cy,(int)(cx+(cr-4)*cos(M_PI/180*h)),(int)(cy-(cr-4)*sin(M_PI/180*h)),ST7735_BLACK);
  h=heading_degrees+90;
  tft.drawLine((int)cx,(int)cy,(int)(cx+(cr-4)*cos(M_PI/180*h)),(int)(cy-(cr-4)*sin(M_PI/180*h)),ST7735_WHITE);
}

void display_info(){
  
  int h=heading_degrees_old;
  String s=String((int)h);
  set_text(103+(h<10?12:(h<100?6:0)),24,String((int)h),ST7735_BLACK,1);
  tft.write(247);
  h=heading_degrees;
  s=String((int)h);
  set_text(103+(h<10?12:(h<100?6:0)),24,s,rgb565(40,40,40),1);
  tft.write(247);
}

void set_text(int x,int y,String text,uint16_t color,byte size){
  
  tft.setTextSize(size);
  tft.setCursor(x,y);
  tft.setTextColor(color);
  tft.print(text);
}

uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b){

  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


void loop(void){
heading += 0.3;
//  MagnetometerScaled scaled=compass.ReadScaledAxis();
  
//  float heading=atan2(scaled.YAxis,scaled.XAxis);

//  MagnetometerScaled scaled = compass.ReadScaledAxis(); //scaled values from compass.
//  float heading = atan2(scaled.YAxis, scaled.XAxis);
//  Vector norm = compass.readNormalize();

  // Calculate heading
 // float heading = atan2(norm.YAxis, norm.XAxis);

//     Serial.print(" HMC5883L");
  //   Serial.println(heading);
//  heading+=declinationAngle;
  
  if(heading<0)heading+=2*PI;
  if(heading>=2*PI)heading-=2*PI;
  
  heading_degrees=heading*180/M_PI+heading_degrees_correction;
  if(heading_degrees<0)heading_degrees+=360;
  if(heading_degrees>=360)heading_degrees-=360;
 //    Serial.println(heading_degrees); 
  if(heading_degrees!=heading_degrees_old){
    
    display_info();
    display_graphic();
    heading_degrees_old=heading_degrees;
  }

   delay(500);
}

void setup(void){

    
pinMode(PIN_PLUS, OUTPUT);      // sets the digital pin as output
pinMode(PIN_BL, OUTPUT); 
digitalWrite(PIN_PLUS, HIGH);
digitalWrite(PIN_BL, HIGH);
  Serial.begin(9600);

 // Wire.begin();
  Serial.println("Initialize HMC5883L");
  // HMC5883L
  // compass = HMC5883L();

  // Display
  tft.initR(INITR_BLACKTAB);
  tft.setTextWrap(true); 
  
  display_show();
   // Set measurement range
   /*
  compass.setRange(HMC5883L_RANGE_1_3GA);
  // Set measurement mode
  compass.setMeasurementMode(HMC5883L_CONTINOUS);
  // Set data rate
  compass.setDataRate(HMC5883L_DATARATE_30HZ);
  // Set number of samples averaged
  compass.setSamples(HMC5883L_SAMPLES_8);
  // Set calibration offset. See HMC5883L_calibration.ino
  compass.setOffset(0, 0);

  */
}
