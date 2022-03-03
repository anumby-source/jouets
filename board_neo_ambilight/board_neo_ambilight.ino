#include <Metro.h>  // library
#include <math.h> 
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN_GND 6
#define PIN 7
#define PIN_VCC 8
#define PIN_GND2 9
// -------------------------VERSION NEOPIXEL INIT avec RGB -----------------

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

Metro ledlightup = Metro(60);
Metro processdata = Metro(50);
Metro TcsTrigger = Metro(10);
 
int s0=12,s1=13,s2=10,s3=11; // port definition of color sensor
int out=2;
int flag=0;  // initialization
int counter=0;
int countR=0,countG=0,countB=0;
int Raverage = 0;
int Baverage = 0;
int Gaverage = 0;

void setup()
{
  Serial.begin(115200);




  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  pinMode(PIN_VCC, OUTPUT);      // sets the digital pin as output
pinMode(PIN_GND, OUTPUT); 
pinMode(PIN_GND2, OUTPUT); 
digitalWrite(PIN_VCC, HIGH);
digitalWrite(PIN_GND, LOW);
digitalWrite(PIN_GND2, LOW);

  colorWipe(strip.Color(255, 0, 0), 20); // Red
  colorWipe(strip.Color(0, 255, 0), 520); // Green
  colorWipe(strip.Color(0, 0, 255), 20); // Blue
  colorWipe(strip.Color(255, 255, 0), 20); // jaune
  colorWipe(strip.Color(0, 255, 255), 20); // Green
  colorWipe(strip.Color(255, 0, 255), 20); // violet
    colorWipe(strip.Color(255, 255, 255), 20); // violet

  pinMode(s0,OUTPUT);
  pinMode(s1,OUTPUT); 
  pinMode(s2,OUTPUT);
  pinMode(s3,OUTPUT);
 
  analogWrite(3,0);
  analogWrite(5,0);
  analogWrite(6,0);
 
}
 
void TCS()
{
  digitalWrite(s0,HIGH);
  digitalWrite(s1,HIGH);
  attachInterrupt(0, ISR_INTO, CHANGE);
 
}
 
void ISR_INTO()
{
  counter++;
}
 

 
void loop()       //   call function
{
  TCS();
  if(TcsTrigger.check()){
    Tcstrigger();
  }
  if(processdata.check()){
    procedata();
  }   
  if(ledlightup.check()){ 
     ledup();

  
  }
 
}
void Tcstrigger()  //   data acquisition
{
      flag++;
    if(flag==1){
 //     digitalWrite(s2,LOW);
  //    digitalWrite(s3,LOW);   
      countR=counter;     
      digitalWrite(s2,HIGH);
      digitalWrite(s3,HIGH);
    }     
    else if(flag==2){
      countG=counter;       
      digitalWrite(s2,LOW);
      digitalWrite(s3,HIGH);  
 
    }
    else if(flag==3){
      countB=counter;    
      digitalWrite(s2,LOW);
      digitalWrite(s3,LOW);   
      flag=0; 
    }
    counter=0; 
}
 
void  procedata()        // data processing
{
      static int Rinput[5] ={
      0,0,0,0,0                }
    ,Binput[5] ={
      0,0,0,0,0                }
    ,Ginput[5] ={
      0,0,0,0,0                };
 
    for(int i = 4;i > 0;i--){
      Rinput[i] = Rinput[i-1];
      Binput[i] = Binput[i-1];
      Ginput[i] = Ginput[i-1];
    }
  
    if(countR < 2500)
      Rinput[0] = countR;
    else
      Rinput[0] = Rinput[1];
 
    if(countB < 2500)
      Binput[0] = countB;
    else
      Binput[0] = Binput[1]; 
 
    if(countG < 2500)
      Ginput[0] = countG;
    else
      Ginput[0] = Ginput[1];    
 
    Raverage = 0;
    Baverage = 0;
    Gaverage = 0;
 
    for(int i = 0;i <= 4;i++){
      Raverage += Rinput[i];
      Baverage += Binput[i];
      Gaverage += Ginput[i];
    }
    Raverage /= 5;
    Baverage /= 5;
    Gaverage /= 5;
       
}
 
void  ledup()     // data output
{
    int ledvalueR = Raverage;
    int ledvalueG = Gaverage;
    int ledvalueB = Baverage;
 
    ledvalueR = constrain(ledvalueR,0,1700);
    ledvalueB = constrain(ledvalueB,0,1500);
    ledvalueG = constrain(ledvalueG,0,1650);
    Serial.print("R:");
    Serial.print(Raverage,DEC); 
    Serial.print(" ");
//    Serial.print(ledvalueR,DEC); 
    Serial.print(" B:");
    Serial.print(Baverage,DEC);
    Serial.print(" ");
//    Serial.print(ledvalueB,DEC);
    Serial.print(" G:");
    Serial.print(Gaverage,DEC);    Serial.print(" ");
    Serial.print(" ");
 //   Serial.println(ledvalueG,DEC);
      Serial.println("");  
   
    ledvalueR = map(ledvalueR,0,1700,0,255);
    ledvalueB = map(ledvalueB,0,1500,0,255);
    ledvalueG = map(ledvalueG,0,1650,0,255);
 
 /*   analogWrite(3,ledvalueR);    
    analogWrite(6,ledvalueB);
    analogWrite(5,ledvalueG); */

   colorWipe(strip.Color(ledvalueR, ledvalueG, ledvalueB), 0); // Red
//  colorWipe(strip.Color(255, 0, 0), 50); // Red
//  colorWipe(strip.Color(0, 255, 0), 50); // Green
//  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

