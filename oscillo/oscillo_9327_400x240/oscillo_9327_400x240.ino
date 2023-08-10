// UTFT_Demo_400x240 (C)2014 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program is a demo of how to use most of the functions
// of the library with a supported display modules.
//
// This demo was made for modules with a screen resolution 
// of 400x240 pixels.
//
// This program requires the UTFT library.
//

#include <UTFT.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <TouchScreen.h>
// Declare which fonts we will be using
extern uint8_t SmallFont[];

// Set the pins to the correct ones for your development shield
// ------------------------------------------------------------
// Arduino Uno / 2009:
// -------------------
// Standard Arduino Uno/2009 shield            : <display model>,A5,A4,A3,A2
// DisplayModule Arduino Uno TFT shield        : <display model>,A5,A4,A3,A2
//
// Arduino Mega:
// -------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
// CTE TFT LCD/SD Shield for Arduino Mega      : <display model>,38,39,40,41
//
// Remember to change the model parameter to suit your display module!
//UTFT myGLCD(ITDB32WD,38,39,40,41);
//UTFT myGLCD(ILI9327_8,38,39,40,41); //3.5" TFTLCD for arduino 2560 from mcufriend.com
UTFT tft(ILI9327_8,A2,A1,A3,A4); //3.5" TFTLCD for arduino 2560 from mcufriend.com on UNO

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//Andersmmg_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;

#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
#define ll 240
#define LL 400
#define UU 360 
// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;
int Sample[UU];
int OldSample[UU];
int x;
int xo;
int interval=0;
int potPin1 = A5;// select the input pins for the potentiometers
char buf[12];
int ButtonDown=1;
int Continue;
const int maxi=1023;
const int mini=0;
const int maxo=ll-7;
const int mino=17;
unsigned long StartTime ;
unsigned long TotalTime ;

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void setup()
{

  tft.InitLCD();
  tft.setFont(SmallFont);
}
void loop()
{
//  tft.setRotation(1);
  tft.setCursor(BOXSIZE, 0);
  tft.setTextColor(currentcolor); 
  digitalWrite(13, HIGH);
    tft.print("Interval ");

 //  tft.setRotation(0);

 //  tft.setCursor(0, 0);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  scanTS( p);
         SampleAndDisplay();

}
void scanTS(TSPoint p){ 
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
  
    // scale from 0->1023 to tft.width
    p.x = (map(p.x, TS_MINX, TS_MAXX, 0, tft.width()));
    p.y = (map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    
    Serial.print("("); Serial.print(p.x);
    Serial.print(", "); Serial.print(p.y);
    Serial.println(")");
    
    
  //  if (p.y < BOXSIZE) {
         if (1) {
       oldcolor = currentcolor;

       if (p.x < BOXSIZE) { 
         currentcolor = RED; 
         tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);interval=0;
       } else if (p.x < BOXSIZE*2) {
         currentcolor = YELLOW;
         tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, WHITE); interval=100;
       } else if (p.x < BOXSIZE*3) {
         currentcolor = GREEN;interval=250;
         tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*4) {
         currentcolor = CYAN;interval=500;
         tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*5) {
         currentcolor = BLUE;1000;
         tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*6) {
         currentcolor = MAGENTA;2000;
         tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, WHITE);
         interval=4000;
       }

       if (oldcolor != currentcolor) {
          if (oldcolor == RED) tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
          if (oldcolor == YELLOW) tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
          if (oldcolor == GREEN) tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
          if (oldcolor == CYAN) tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
          if (oldcolor == BLUE) tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
          if (oldcolor == MAGENTA) tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
       }
    }
  }
}
void SampleAndDisplay()
{
  // FASTADC
  bitSet(ADCSRA,ADPS2);
  bitClear(ADCSRA,ADPS1);
  bitClear(ADCSRA,ADPS0);  
 //  tft.setRotation(1);
//   while( (analogRead(potPin1) < 200));
   if(interval){
  for (int i=0; i<UU; i+=1)
  {
    Sample[i]=(analogRead(potPin1));
    delayMicroseconds(interval); 
  }
  }else for (int i=0; i<UU; i+=1) Sample[i]=(analogRead(potPin1));
    Sample[0]=map(Sample[0], mini, maxi, maxo, mino);
  for (x=1; x<UU; x++)
  {
        Sample[x]=map(Sample[x], mini, maxi, maxo, mino);
    xo=x-1; int x1=x+BOXSIZE;
    tft.drawLine(x1,OldSample[xo],x1,OldSample[x],BLACK);
    tft.drawLine(x1,Sample[xo],x1,Sample[x],WHITE); 
    OldSample[xo]=Sample[xo];  
  }
//  OldSample[UU-2]=Sample[UU-2];
  OldSample[UU-1]=Sample[UU-1];
} 
