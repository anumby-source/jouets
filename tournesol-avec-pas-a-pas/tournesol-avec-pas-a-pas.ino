// Tournesol avec moteur pas à pas et une cellule 0.5 Volts
// arnaudrco https://github.com/arnaudrco/exemples/blob/main/README.md
// placer la cellule entre A6 et la masse (GND)
// le moteur pas à pas effectue plusiers itérations 
// le maximum d'ensoleillement est trouvé pour chaque itération
// il avance d'un quart de tour, avance ou recule si le maximum est respectivment au dessus ou en dessous du seuil

// Define stepper motor connections and steps per revolution:
#define dirPin 2
#define stepPin 5
#define enablePin 8

#define delai 2000
#define stepsPerRevolution 200
#define stepsPerScan 50 // nombre de pas par itération
#define seuil1 15 // nombre de pas pour le seuil bas
#define seuil2 35 // seuil haut

#define photoPin A6

int dir=1; // direction
       
void setup() {
  Serial.begin(115200);
    Serial.println("Tracker solaire : PhotoVoltaique.");
  // Declare pins as output:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
    pinMode(enablePin, OUTPUT);
        digitalWrite(enablePin, LOW);
//       pinMode(photoPin, INPUT_PULLUP);
pinMode(photoPin, INPUT);

int i=3; // nombre d'itérations de scan pour le moteur
while (i--){
  int m=maximum(stepsPerScan);
  if (m>seuil2){
             Serial.println("Tracker solaire : continue !");
  } else if(m>seuil1){
              Serial.println("Tracker solaire : position trouvée !");
      dir=1-dir; maximum(stepsPerScan-m);// recule

      i=0; // boucle finie ! 
      break;
    } else {
      dir=1-dir; maximum(stepsPerScan);// recule complétement
          Serial.println("Tracker solaire : change de sens");
    }
  
  
}
}
void loop() {
  // placer ici votre boucle de controle du moteur 
               digitalWrite(enablePin, HIGH); // mise en veille du moteur
                delay(1000);
               
}   
int maximum(int n) { // retourne la position du maximum sur n steps 
    int photoMax = analogRead( photoPin ) ;
    int pos=0;
       digitalWrite(enablePin, LOW);
         digitalWrite(dirPin, dir);
  // Spin the stepper motor 1 revolution slowly:
  for (int i = 0; i < n; i++) {
    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delai);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delai);
    // maximum
      if (analogRead( photoPin )>photoMax ){
    photoMax = analogRead( photoPin );
    pos=i;
  }
  }
 Serial.print("pos :" );
 Serial.print(pos);
 Serial.print("photoMax :" );
   Serial.println(photoMax );
  return( pos);
}  
