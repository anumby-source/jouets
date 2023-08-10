// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

#include <Adafruit_GFX.h>    // Core graphics library
#include "Andersmmg_TFTLCD.h" // Hardware-specific library
#include <TouchScreen.h>
// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Andersmmg_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
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


void setup(void) {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));

#ifdef USE_ADAFRUIT_SHIELD_PINOUT
  Serial.println(F("Using Adafruit 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Adafruit 2.8\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());
  tft.reset();
  uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
 //   return;
 identifier = 0x9341;
  }

  tft.begin(identifier);
  
    Serial.println(F("OScillo!"));

  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
  currentcolor = RED;
  pinMode(13, OUTPUT);
   tft.setRotation(1);
   for (int i=BOXSIZE+9; i<LL; i+=10)
  {
    tft.drawLine(i, 12, i, 16,WHITE);
    tft.drawLine(i, ll-5, i, ll-1,WHITE);
  }
}
void loop()
{
  tft.setRotation(1);
  tft.setCursor(BOXSIZE, 0);
  tft.setTextColor(currentcolor); 
  digitalWrite(13, HIGH);
    tft.print("Interval ");

   tft.setRotation(0);

   tft.setCursor(0, 0);
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
   tft.setRotation(1);
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
