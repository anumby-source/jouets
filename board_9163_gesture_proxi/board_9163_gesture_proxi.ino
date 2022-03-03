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

#define APDS9960_INT    2 // Needs to be an interrupt pin
SparkFun_APDS9960 apds = SparkFun_APDS9960();

int isr_flag = 0;

uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;
uint16_t old_ambient_column = 0;
uint16_t old_red_column = 0;
uint16_t old_green_column = 0;
uint16_t old_blue_column = 0;
uint16_t detectedcolor = 0;
uint16_t tfth = 128;

#define PIN_BL 3
#define SCL   4
#define SDA   5
#define RS   6
#define RST    7
#define CS 8
#define PIN_GND 9
#define PIN_VCC 10

//TFT_ILI9163C tft = TFT_ILI9163C(__CS, __DC);
// TFT_ILI9163C tft = TFT_ILI9163C(CS,RS, SDA, SCL, RST);
TFT_ILI9163C tft = TFT_ILI9163C(CS,RS, RST );
uint8_t proximity_data = 0;
uint8_t proxi=0;

void interruptRoutine() {
  isr_flag = 1;
}

void setup() {
    Serial.begin(9600);
  // Set interrupt pin as input
  pinMode(APDS9960_INT, INPUT);
  // Initialize Serial port
  Serial.begin(9600);
   pin9163();
  Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("SparkFun APDS-9960 - GestureTest"));
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
  /*
  // ----------------- si lumière faible >> COULEUR
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
  
  //-------------------- COULEUR
  if(ambient_light < 100){
      proxi=1;
  }
  else{
      proxi=0;
      if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
      } else { Serial.println(F("Something went wrong during gesture sensor init!"));}
    }
    */
    // Initialize interrupt service routine
  attachInterrupt(0, interruptRoutine, FALLING);
  

  tft.begin();
    tft.setRotation(0);
//  if(proxi) { tft.fillScreen(RED);}
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
if(proxi==0){

  if(ambient_light < 100){
      proxi=1;
  } else { 
      proxi=2;
        if ( apds.enableGestureSensor(true) ) {
            Serial.println(F("Gesture sensor is now running"));
      } else { Serial.println(F("Something went wrong during gesture sensor init!"));}
    }
  }

  
 if(proxi==1){//------------------- COULEUR
  
  detectedcolor = rgb(red_light,green_light,blue_light);
  tft.drawCircle(100 , 100, 16, WHITE);
  tft.fillCircle(100 , 100, 15, detectedcolor);
//  tftMediaButtonsTest();
  bar ();
  delay(100);
  return;

} else{//------------------GESTURE
    // Start running the APDS-9960 gesture sensor engine
 
 if( isr_flag == 1 ) {
    detachInterrupt(0);
    handleGesture();
    isr_flag = 0;
    attachInterrupt(0, interruptRoutine, FALLING);
  } 
}
}
void handleGesture() {
    if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        Serial.println("UP");
          boutonBas(0);
               break;
      case DIR_DOWN:
        Serial.println("DOWN");
      boutonHaut(0);    
        break;
      case DIR_LEFT:
        Serial.println("LEFT");
        boutonHaut(1);
        break;
      case DIR_RIGHT:
        Serial.println("RIGHT");
        boutonBas(1);
        break;
      case DIR_NEAR:
        Serial.println("NEAR");
        boutonRond(); 
        break;
      case DIR_FAR:
        Serial.println("FAR");
        boutonCarre(CYAN);
        break;
      default:
        Serial.println("NONE");
              boutonCarre(YELLOW);
    }
  }
}

uint16_t rgb( uint16_t r, uint16_t g , uint16_t b){
  uint16_t vmax=r;// normalisation
  if(g>vmax)  vmax=g;
  if(b>vmax)  vmax=b;
  r= (255 * r)/vmax;g= (255 * g)/vmax;b= (255 * b)/vmax;
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
void bar (){
      uint8_t width = 15;
      uint16_t xstart = 10;
      uint16_t maxval = 0;           // Skalierung (max rgb wert)
      uint16_t maxheight = tfth ; // Maximale Höhe der Balken in Pixel
      uint32_t bsize = 0;           // Balkenhöhe in Pixel
 
      maxval = ambient_light;
      double scaling_factor = ((double)maxheight / maxval);

      bsize = (scaling_factor * red_light);
      if (old_red_column > bsize) {
        tft.fillRect(xstart , tfth - old_red_column, width, old_red_column - bsize, BLACK);
      } else {
        tft.fillRect(xstart , tfth - bsize, width, bsize - old_red_column, RED);
      }
      old_red_column = bsize;
      xstart += width;

      bsize = (scaling_factor * green_light);
      if (old_green_column > bsize) {
        tft.fillRect(xstart , tfth - old_green_column, width, old_green_column - bsize, BLACK);
      } else {
        tft.fillRect(xstart , tfth - bsize, width, bsize - old_green_column, GREEN);
      }
      old_green_column = bsize;
      xstart += width;

      bsize = (scaling_factor * blue_light);
      if (old_blue_column > bsize) {
        tft.fillRect(xstart , tfth - old_blue_column, width, old_blue_column - bsize, BLACK);
      } else {
        tft.fillRect(xstart , tfth - bsize, width, bsize - old_blue_column, BLUE);
      }
      old_blue_column = bsize;
    }
  
void tftMediaButtonsTest(void) 
{
 boutonRond(); 
boutonHaut(0);
boutonBas(0);
boutonHaut(1);
boutonBas(1);

}
void boutonHaut(int rot) {
  tft.setRotation(rot); // Set to landscape   
//  tft.fillScreen(BLACK);
  tft.fillRoundRect(32, 32, 64, 64, 8, WHITE);
  tft.fillTriangle(48, 48, 80, 48, 64, 80, RED);
}
void boutonBas(int rot) {
  tft.setRotation(rot); // Set to landscape   
//  tft.fillScreen(BLACK);
  tft.fillRoundRect(32, 32, 64, 64, 8, WHITE);
    tft.fillTriangle(48, 80, 80, 80, 64, 48, GREEN);
} 
void boutonRond() {
//  tft.setRotation(rot); // Set to landscape   
//  tft.fillScreen(BLACK);
  tft.fillRoundRect(32, 32, 64, 64, 8, WHITE);
    tft.fillCircle(64, 64, 16, MAGENTA);
}
void boutonCarre(int color) {
//  tft.setRotation(rot); // Set to landscape   
//  tft.fillScreen(BLACK);
  tft.fillRoundRect(32, 32, 64, 64, 8, color);
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

