#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include <Wire.h>
#include <SparkFun_APDS9960.h>

// Color definitions
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF

// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;

/*
Teensy3.x and Arduino's
You are using 4 wire SPI here, so:
 MOSI:  11//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 SCK:   13//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 the rest of pin below:
 */




#define PIN_BL 3
#define SCL   4
#define SDA   5
#define RS   6
#define RST    7
#define CS 8
#define PIN_GND 9
#define PIN_VCC 10

/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
Arduino's 8 bit: any
DUE: check arduino site
If you do not use reset, tie it to +3V3
*/


//TFT_ILI9163C tft = TFT_ILI9163C(__CS, __DC);
// TFT_ILI9163C tft = TFT_ILI9163C(CS,RS, SDA, SCL, RST);
TFT_ILI9163C tft = TFT_ILI9163C(CS,RS, RST );

void setup() {
    Serial.begin(9600);
  Serial.println(F("--------------------------------"));
  Serial.println(F("SparkFun APDS-9960 - ColorSensor"));
  Serial.println(F("--------------------------------"));
  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  } 
  // Start running the APDS-9960 light sensor (no interrupts)
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during light sensor init!"));
  }
  pin9163();
  tft.begin();
    tft.setRotation(1);
    


}

void loop(){
  uint16_t color;
  // Read the light levels (ambient, red, green, blue)
  if (  !apds.readAmbientLight(ambient_light) ||
        !apds.readRedLight(red_light) ||
        !apds.readGreenLight(green_light) ||
        !apds.readBlueLight(blue_light) ) {
    Serial.println("Error reading light values");
  } else {
    Serial.print("Ambient: ");
    Serial.print(ambient_light);
    Serial.print(" Red: ");
    Serial.print(red_light);
    Serial.print(" Green: ");
    Serial.print(green_light);
    Serial.print(" Blue: ");
    Serial.println(blue_light);
  }
//  testLines(random(0x00ff,0xffff));
//  delay(100);
  testText(rgb(red_light,green_light,blue_light));
  delay(500);
}


unsigned long testText(uint16_t color) {
 // tft.fillScreen();

        tft.fillRect(0, 0, 128, 128,color);

        
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  
  tft.setTextSize(2);
  tft.println("ADPS 9960");
    tft.println(ambient_light);
    
  tft.setTextColor(GREEN); 
  tft.println(green_light);

    tft.setTextColor(BLUE); 
  tft.println(blue_light);

    tft.setTextColor(RED); 
  tft.println(red_light);

  
}

uint16_t rgb( uint16_t r, uint16_t g , uint16_t b){
   if(r>255)  r=255;
   if(g>255)  g=255;
   if(b>255)  b=255;
  return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
}
void pin9163(){
  pinMode(PIN_VCC, OUTPUT);      // sets the digital pin as output
  pinMode(PIN_BL, OUTPUT); 
  pinMode(PIN_GND, OUTPUT); 
  digitalWrite(PIN_VCC, HIGH);
  digitalWrite(PIN_BL, HIGH);
  digitalWrite(PIN_GND, LOW);
  pinMode(SCL, INPUT); 
  pinMode(SDA, INPUT); 
}


