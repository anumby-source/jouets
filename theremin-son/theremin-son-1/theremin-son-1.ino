/* Code de l'Arduino 1 - capteurs ultra son + resistance photo
code sous license GPL*/

// modele 1 tons


/* si bargraph clignotant, 2 sorties en plus (ou 1 si vumètre sur 1,25V) et ajouter ces lignes
UoutABG=map(lecture_echoA,290,3480,0,64);
pinMode(3,OUTPUT);
analogWrite(3,UoutABG);
*/
const uint8_t analogOutPin = 8; // valeur 8  (buzzer)
const uint8_t buzzer = 2; // valeur 8  (buzzer)


// capteur amplitude
int trigA = 5; 
int echoA = 6; 
long lecture_echoA; 
int UoutA;
long satA=6000;
float distmm;

// capteur fréquence
int trigF = 3; 
int echoF = 4; 
long lecture_echoF; 
int UoutF, UoutF2;
int satF=6000;
float distmmF;

int dist_mini=200;



void setup() 
{ 
 pinMode(trigA, OUTPUT); // setup capteur amplitude
  digitalWrite(trigA, LOW); 
  pinMode(echoA, INPUT); 

 pinMode(trigF, OUTPUT); // setup capteur freq
  digitalWrite(trigF, LOW); 
  pinMode(echoF, INPUT);

   
 Serial.begin(115200);  
}


void checkAmpl () // fonction check amplitude
{
  digitalWrite(trigA, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigA, LOW); 
  lecture_echoA = pulseIn(echoA, HIGH); 
  
  if(lecture_echoA < dist_mini) 
  {
  UoutA=0;
  distmm=0;
  }
  else if((lecture_echoA > 3480) && (lecture_echoA > satA))
    {
    UoutA=0;
    distmm=0;
    }
      else if ((lecture_echoA >= 3480) && (lecture_echoA<= satA))
      {
      UoutA=255;
      distmm=600;
      }
  else
  {
  UoutA=map(lecture_echoA,200,3480,0,255); // converti la distance en 8 bits pour la sortie en PWM --> conversion echo en mm = /5.8 ici max 600 mm
  distmm=(lecture_echoA/5.8);  
  }
  Serial.print("echo Amp: "); 
  Serial.println(lecture_echoA);
  Serial.print("tension: "); 
  Serial.println(UoutA);
  Serial.print("distance: "); 
  Serial.println(distmm);
  Serial.println("----------------------------------------------------------------------");
   UoutF2=map(lecture_echoA,200,3480,0,255); // converti la distance en 8 bits pour la sortie en PWM --> conversion echo en mm = /5.8 ici max 600 mm
}

void checkFreq ()
{
  digitalWrite(trigF, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigF, LOW); 
  lecture_echoF = pulseIn(echoF, HIGH); 
  
  if(lecture_echoF < dist_mini) 
  {
  UoutF=0;
  distmmF=0;
  }
  else if((lecture_echoF > 3480) && (lecture_echoF > satF))
    {
    UoutF=0;
    distmmF=0;
    }
      else if ((lecture_echoF >= 3480) && (lecture_echoF<= satF))
      {
      UoutF=255;
      distmmF=600;
      }
  else
  {
  UoutF=map(lecture_echoF,200,3480,0,255); // converti la distance en 8 bits pour la sortie en PWM --> conversion echo en mm = /5.8 ici max 600 mm
  distmmF=(lecture_echoF/5.8);  
  }
  Serial.print("echo freq: "); 
  Serial.print(lecture_echoF);
  Serial.print("tension: "); 
  Serial.print(UoutF);
  Serial.print(" distance: "); 
  Serial.println(distmmF);
  //Serial.println("///////////////////////////////////////////////////////////////////////////////////////////////");
} 

void loop() 
{ 
//a=millis();
checkFreq();
//checkAmpl();
delay(100);
//Serial.print("Temps de cycle : "); 
 // Serial.println(millis()-a);
//checkFreq();


//  frequence = map(luminosite, 0, 1023, 50, 30000);//reduire la fourchette par essais successifs
  // Conversion tension en fréquence :
// tone(analogOutPin, UoutF);
 tone(buzzer, UoutF);
//tone(analogOutPin, 100);
//tone(buzzer, 200);

 
  
}
