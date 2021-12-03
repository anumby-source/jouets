#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
//#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

#define capteur A0
#define PinA 0 // broche enable du L298N pour le premier moteur
#define PinB 2 //  broche enable du L298N pour le deuxième moteur
#define SpeedA 5 // Premier moteur
#define SpeedB 4 // Deuxième moteur
#define LED1        D6 // On Trinket or Gemma, suggest changing this to 1
#define LED2        D7    // On Trinket or Gemma, suggest changing this to 1

#define NB_ITERATION        100  // itérations de la fonction delta   

// On Trinket or Gemma, suggest changing this to 1
#define DROITE 1 // tourne à droite
#define GAUCHE 2 // tourne à gauche
//#define MILIEU 510 // pont de résistance 1024 / 2 environ
#define MAX 12 // sensibilite maximum avec réglage initial MAX / 2
#define MIN 2 // sensibilite min
#define DD        2  // delta pour la sensibilité    
//const int sensibilite = MAX; // écart admisensibiliteible par rapport à la valeur initiale
int initial ; // valeur initiale du capteur balance lumière
int dir= 0; // direction 0 : tout droit

IPAddress    apIP(44, 44, 44, 44);      // Définition de l'adresse IP statique.
// const char *ssid = "RCO";      // Nom du reseau wifi (*** A modifier ***)
const char *password = "12345678";      // mot de pasensibilitee       (*** A modifier ***)
//ESP8266WebServer server(80);   

 int AA=500; // target Speed
 int aa=0,bb=0; // vitesse moteur
// int AA=00; // target Speed
int sensibilite ; // sensibilité
char buffer[30];
unsigned long tempoLampe = 0;        // will store last time LAMPE was updated
const long intervalLampe = 2000; 


WiFiServer server(80);

String html1 ="<!DOCTYPE html> \
<html> \
<head> \
<style> .button { padding:10px 10px 10px 10px; width:100%;  background-color: green; font-size: 500%;color:white;} </style>\
<center><h1>ARNAUD";
String html2 ="</h1> \
<form> \
<TABLE BORDER> \
 <TR> <TD> <button name='LED0' class='button' value='1' type='submit'><<</button> </TD> <TD> <button name='LED0' class='button'  value='2' type='submit'>^</button></TD> <TD> <button name='LED0' class='button' value='3' type='submit'>>></button> </TD> </TR> \
 <TR> <TD> <button name='LED0' class='button'  value='4' type='submit'><</button>  </TD> <TD> <button name='LED0' class='button'  value='5' type='submit'>ooo </button></TD> <TD> <button name='LED0' class='button' value='6' type='submit'>></button> </TD> </TR>\
 <TR> <TD> <button name='LED0' class='button'  value='7' type='submit'><<</button>  </TD> <TD> <button name='LED0' class='button'  value='8' type='submit'>v</button></TD> <TD> <button name='LED0' class='button'  value='9' type='submit'>>></button> </TD> </TR>\
</TABLE> \
</form> \
</center>\
</head> \
</html>";
void setup()
{

unsigned long currentMillis = millis();

  char ssid[30];
 sprintf(ssid, "RCO_%d", ESP.getChipId());
 
Serial.begin(115200);
pinMode(PinA, OUTPUT);
pinMode(PinB, OUTPUT);
pinMode(SpeedA, OUTPUT);
pinMode(SpeedB, OUTPUT);
digitalWrite(PinA, LOW);
digitalWrite(PinB, LOW);
  pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    // declaration du wifi:
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
  
  server.begin();
  bip();  // test moteur
   initial = analogRead(capteur);
         if (initial> 20){   // lecture des valeurs initiales (on suppose que les capteurs sont de part et d'autre de la ligne)
             bip();
         }
     sensibilite=MAX / 2; // sensibilite maximum avec réglage initial
       delay(1000);
}
int delta() { // difference entre leds
  long RR=0 ;
  int i= NB_ITERATION;

  digitalWrite(LED1, HIGH);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED2, LOW);   // turn the LED on (HIGH is the voltage level)
//  delayMicroseconds(300); 
      while (i--) RR += analogRead(capteur); 
        Serial.print(RR/NB_ITERATION);
                Serial.print(" ");
                      // wait for a second
  digitalWrite(LED1, LOW);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED2, HIGH);   // turn the LED on (HIGH is the voltage level)
 // delayMicroseconds(100);  
      i= NB_ITERATION;while (i--) RR -= analogRead(capteur); 
          Serial.println(RR/NB_ITERATION);
               
  digitalWrite(LED1, LOW);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED2, LOW); 
 return ( RR/NB_ITERATION );
             
}
void bip(){ // test moteur 
        digitalWrite(PinA, LOW);digitalWrite(PinB, LOW);
               analogWrite(SpeedA,AA);analogWrite(SpeedB,AA);
                           delay(200);
                analogWrite(SpeedA,0);analogWrite(SpeedB,0); 
                 delay(400);          
}
void patinage(){ // 2 moteurs en marche avant pour éviter le patinage
        digitalWrite(PinA, LOW);digitalWrite(PinB, LOW);
               analogWrite(SpeedA,AA);analogWrite(SpeedB,AA);
                           delay(1);         
}
// initialisation de la vitesse
void set_speed(int a, int b){
                aa=a;bb=b;
                analogWrite(SpeedA,aa);analogWrite(SpeedB,bb);
}
void set_stop(){
                analogWrite(SpeedA,0);analogWrite(SpeedB,0);
}
void set_start(){
                analogWrite(SpeedA,aa);analogWrite(SpeedB,bb);
}
void loop()
{ // temporisation de 2s pour moteur
//  analogWrite(SpeedA, 0); analogWrite(SpeedB, 0);
  set_stop();
  int ddd = delta();
  set_start();
    unsigned long currentMillis = millis();
 if (abs(ddd) < sensibilite)  {
    if(dir!= 0){ 
      Serial.println(ddd);Serial.println("tout droit");
      dir=0; tempoLampe=currentMillis;
 //          digitalWrite(PinA, LOW);digitalWrite(PinB, LOW); analogWrite(SpeedA, AA);analogWrite(SpeedB, AA);
    }
      if ( ddd > 0 ) {
                  digitalWrite(PinA, LOW);digitalWrite(PinB, LOW); 
                  set_speed(  map(ddd, 0, sensibilite, AA, 0), AA );
    } else if (ddd < 0) {
           digitalWrite(PinA, LOW);digitalWrite(PinB, LOW); 
                             set_speed(  AA, map(ddd, 0, sensibilite, AA,0) );
               // digitalWrite(PinA, LOW);digitalWrite(PinB, HIGH); analogWrite(SpeedA, AA); digitalWrite(SpeedB, AA);
    }
   


  } else {
    
    if ( ddd > 0 ) {
      if(dir!= GAUCHE){ 
      Serial.print(ddd);Serial.println("on tourne à gauche");
      dir=GAUCHE; tempoLampe=currentMillis;  // patinage();
                  // digitalWrite(PinA, LOW);digitalWrite(PinB, LOW); set_speed(0,AA);
                   digitalWrite(PinA, HIGH);digitalWrite(PinB, LOW); set_speed(AA,AA);
    }

  }   else if (ddd < 0) {
    if(dir!= DROITE){ 
     Serial.print(ddd);Serial.println("on tourne à droite");
         dir=DROITE; tempoLampe=currentMillis;  // patinage();
         // digitalWrite(PinA, LOW);digitalWrite(PinB, LOW); set_speed(AA,0);
                digitalWrite(PinA, LOW);digitalWrite(PinB, HIGH); set_speed(AA,AA);
    }

  }

 
    
  }
     if(currentMillis - tempoLampe > intervalLampe) {
          Serial.println("on s'arrête");
            set_speed(0,0);
   } 
  
WiFiClient client=server.available();
if(client){
String request = client.readStringUntil('\r');
//-----------------PAVE HAUT------------
if(request.indexOf("LED0=1") != -1) { 
  AA +=50; if(AA>1023) AA=1023;
}
if(request.indexOf("LED0=2") != -1){ 
  tempoLampe=currentMillis;
  digitalWrite(PinB, LOW);digitalWrite(PinA, LOW);
  analogWrite(SpeedA,AA);analogWrite(SpeedB,AA);
}
if(request.indexOf("LED0=3") != -1){
  sensibilite +=DD; if(sensibilite>MAX) sensibilite=MAX;
  
}
//-----------------PAVE CENTRE------------
if(request.indexOf("LED0=4") != -1){
  tempoLampe=currentMillis;
  digitalWrite(PinA, LOW); digitalWrite(PinB, HIGH);
  analogWrite(SpeedA,AA);analogWrite(SpeedB,AA);
}
if(request.indexOf("LED0=5") != -1){
  analogWrite(SpeedA,0); analogWrite(SpeedB,0);
}
if(request.indexOf("LED0=6") != -1){
  tempoLampe=currentMillis;
  digitalWrite(PinA, HIGH); digitalWrite(PinB, LOW);
  analogWrite(SpeedA,AA);analogWrite(SpeedB,AA);
}
//-----------------PAVE BAS------------
if(request.indexOf("LED0=7") != -1){
 AA -=50; if(AA<0) AA=0;
}
if(request.indexOf("LED0=8") != -1){
  tempoLampe=currentMillis;
  digitalWrite(PinA, HIGH);digitalWrite(PinB, HIGH);
  analogWrite(SpeedA,AA);analogWrite(SpeedB,AA);
}
if(request.indexOf("LED0=9") != -1){
 sensibilite -=DD; if(sensibilite<MIN) sensibilite=MIN;
}
// Affichage de la vitesensibilitee
sprintf(buffer, " M=%d (%d) B=%d", AA, ddd,sensibilite);
//Serial.println(buffer);
client.print(html1);
client.print(buffer);
client.print(html2);
// client.print(html);
request="";
}

  delay(4);
}
