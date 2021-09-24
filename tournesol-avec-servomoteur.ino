// Tournesol 1 axe 2 cellules solaires
// arnaudrco https://github.com/arnaudrco/exemples/blob/main/README.md
// placer une resistance de 20 à 50 KOhms entre A6 et la masse (GND)
// placer les 2 cellules tête-bêche entre A6 et A7
// seuil d'hysteresis réglé pour un arduino nano3 avec convertisseur 12 bits

#include <Servo.h>

#define photoPin1 A6
#define photoPin2 A7
#define seuil 150 // seuil  hysteresis

static const int plusPin = D2;
static const int servoPin = D3;
Servo servo1;
int pos=0;

void setup() {
  pinMode(photoPin1, INPUT_PULLUP);
  pinMode(photoPin2, INPUT);

   pinMode(plusPin, OUTPUT);
   digitalWrite(plusPin, HIGH);
   
    Serial.begin(115200);
    servo1.attach(servoPin);
    Serial.println("Tracker solaire : PhotoVoltaique asservi");
       
}

void loop() {
  // put your main code here, to run repeatedly:
    int p1 = analogRead( photoPin1 ) ;
        int p2 = analogRead( photoPin2 ) ;
                  int delta= p2 - p1;
      Serial.print( p1);Serial.print(" ");Serial.println( delta);
      if(abs(delta)> seuil){
        if (p1 > p2) pos++; 
        else pos--;
      }
        
        if(pos>180) pos=180;
         if(pos<0) pos=0;
           servo1.write(pos);
            delay(10);
}
