/* Jacadi a dit

arnaudrco https://github.com/arnaudrco/exemples/blob/main/README.md

placer une led NEOPIXEL sur pin A5, le buzzer sur pin A4

par rapport au code de Robert Spann :

  le nombre de boutons est programmable

  le nombre de couleurs est programmable

Originaly made by Robert Spann
Code trimmed and sound effects added by digimike
Buttons are to be set on there designated pins without pull down resistors >> PAS NECESSAIRE
and connected to ground rather then +5. 

*/
#include <Adafruit_NeoPixel.h>
#define PIN        A5 // neo
#define NUMPIXELS 1 // Popular NeoPixel ring size
#define NBLED 4
#define NOIR -1
#define BLANC 4
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); 

#include <Tone.h>
Tone speakerpin;
int starttune[] = {NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_F4, NOTE_G4};
int duration2[] = {100, 200, 100, 200, 100, 400, 100, 100, 100, 100, 200, 100, 500};
int note[] = {NOTE_C4, NOTE_C4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_C5};
int duration[] = {100, 100, 100, 300, 100, 300};
/* {6, 7, 8, 11} echangé 
boolean button[] = {6, 7, 8, 11}; //The four button input pins
boolean ledpin[] = {2, 3, 4, 5} ;  */
int button[] = {6, 7, 8, 9}; //The four button input pins
int turn = 0;  // turn counter
int buttonstate = 0;  // button state checker
int randomArray[100]; //Intentionally long to store up to 100 inputs (doubtful anyone will get this far)
int inputArray[100];  

void neo(int color){ 
  switch(color){
       case NOIR :// G R B
         pixels.setPixelColor(0, pixels.Color(0, 0, 0)); break;
        case 0 :// rouge
          pixels.setPixelColor(0, pixels.Color(0, 150, 0)); break;
        case 1 ://jaune
          pixels.setPixelColor(0, pixels.Color(150, 250, 0)); break;
        case 2 :// bleu
          pixels.setPixelColor(0, pixels.Color(0, 0, 150)); break;
        case 3 ://vert
          pixels.setPixelColor(0, pixels.Color(150, 0, 0)); break;
       case BLANC ://blanc
          pixels.setPixelColor(0, pixels.Color(150, 150, 150)); break;
  }
    pixels.show();   // Send the updated pixel colors to the hardware.
}
void setup() 
{
      pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
             
  Serial.begin(112500);
  speakerpin.begin(A4); // speaker is on pin 11
        Serial.println("Simon 3 LEDs");
  for(int x=0; x<NBLED; x++)  // LED pins are outputs
  {
          Serial.println(x);
  }
  for(int x=0; x<NBLED; x++) 
  {
    pinMode(button[x], INPUT);  // button pins are inputs
    digitalWrite(button[x], HIGH);  // enable internal pullup; buttons start in high position; logic reversed
  }
  randomSeed(analogRead(0)); //Added to generate "more randomness" with the randomArray for the output function
  for (int thisNote = 0; thisNote < 13; thisNote ++) {
     // play the next note:
     speakerpin.play(starttune[thisNote]);
     // hold the note:
     if (thisNote==0 || thisNote==2 || thisNote==4 || thisNote== 6)
     {
       neo(0);// digitalWrite(ledpin[0], HIGH);
     }
     if (thisNote==1 || thisNote==3 || thisNote==5 || thisNote== 7 || thisNote==9 || thisNote==11)
     {
       neo(1); // digitalWrite(ledpin[1], HIGH);
     }
     if (thisNote==8 || thisNote==12)
     {
       neo(2);//digitalWrite(ledpin[2], HIGH);
     }  
     if (thisNote==10)
     {   
       neo(3);//digitalWrite(ledpin[3], HIGH);
     }
     delay(duration2[thisNote]);
     // stop for the next note:
         neo(NOIR);
     speakerpin.stop();
     delay(25);
    }
  delay(1000);
}
 
void loop() 
{   
  for (int y=0; y<=99; y++)
  {
    //function for generating the array to be matched by the player
   neo(BLANC);
    for (int thisNote = 0; thisNote < 6; thisNote ++) {
     // play the next note:
     speakerpin.play(note[thisNote]);
     // hold the note:
     delay(duration[thisNote]);
     // stop for the next note:
     speakerpin.stop();
     delay(25);
    }
     neo(NOIR);
    delay(1000);
  
    for (int y=turn; y <= turn; y++)
    { //Limited by the turn variable
      Serial.println(""); //Some serial output to follow along
      Serial.print("Turn: ");
      Serial.print(y);
      Serial.println("");
      randomArray[y] = random(1, NBLED+1); //Assigning a random number (1-4) to the randomArray[y], y being the turn count
      for (int x=0; x <= turn; x++)
      {
        Serial.print(randomArray[x]);
      
        for(int y=0; y<NBLED; y++)
        {
   //       if (randomArray[x] == 1 && (y == 0) )
          {  //if statements to display the stored values in the array
            neo(0); //digitalWrite(ledpin[y], HIGH);
            speakerpin.play(NOTE_G3, 100);
            delay(400);
                 neo(NOIR); // digitalWrite(ledpin[y], LOW);
            delay(100);
          }

          if (randomArray[x] == 2 && (y == 1) )
          {
            neo(1);//digitalWrite(ledpin[y], HIGH);
            speakerpin.play(NOTE_A3, 100);
            delay(400);
                 neo(NOIR); //digitalWrite(ledpin[y], LOW);
            delay(100);
          }
  
          if (randomArray[x] == 3 && (y == 2) ) 
          {
            neo(2);//digitalWrite(ledpin[y], HIGH);
            speakerpin.play(NOTE_B3, 100);
            delay(400);
                 neo(NOIR); //digitalWrite(ledpin[y], LOW);
            delay(100);
          }

          if (randomArray[x] == 4 && (y == 3) )
          {
            neo(3);//digitalWrite(ledpin[y], HIGH);
            speakerpin.play(NOTE_C4, 100);
            delay(400);
                 neo(NOIR); // digitalWrite(ledpin[y], LOW);
            delay(100);
          }
        }
      }
    }
    input();
  }
} 
void input() { //Function for allowing user input and checking input against the generated array

  for (int x=0; x <= turn;)
  { //Statement controlled by turn count

    for(int y=0; y<NBLED; y++)
    {
      
      buttonstate = digitalRead(button[y]);
      if (buttonstate == LOW && button[y] == button[0])
      { //Checking for button push
        neo(0); // digitalWrite(ledpin[0], HIGH);
        speakerpin.play(NOTE_G3, 100);
        delay(200);
        neo(NOIR); // digitalWrite(ledpin[0], LOW);
        inputArray[x] = 1;
        delay(250);
        Serial.print(" ");
        Serial.print(1);
        if (inputArray[x] != randomArray[x]) { //Checks value input by user and checks it against
          fail();                              //the value in the same spot on the generated array
        }                                      //The fail function is called if it does not match
        x++;
      }
       if (buttonstate == LOW && button[y] == button[1])
      {
        neo(1); // digitalWrite(ledpin[1], HIGH);
        speakerpin.play(NOTE_A3, 100);
        delay(200);
        neo(NOIR); //digitalWrite(ledpin[1], LOW);
        inputArray[x] = 2;
        delay(250);
        Serial.print(" ");
        Serial.print(2);
        if (inputArray[x] != randomArray[x]) {
          fail();
        }
        x++;
      }

      if (buttonstate == LOW && button[y] == button[2])
      {
        neo(2); // digitalWrite(ledpin[2], HIGH);
        speakerpin.play(NOTE_B3, 100);
        delay(200);
        neo(NOIR); // digitalWrite(ledpin[2], LOW);
        inputArray[x] = 3;
        delay(250);
        Serial.print(" ");
        Serial.print(3);
        if (inputArray[x] != randomArray[x]) {
          fail();
        }
        x++;
      }

      if (buttonstate == LOW && button[y] == button[3])
      {
        neo(3); // digitalWrite(ledpin[3], HIGH);
        speakerpin.play(NOTE_C4, 100);
        delay(200);
        neo(NOIR); // digitalWrite(ledpin[3], LOW);
        inputArray[x] = 4;
        delay(250);
        Serial.print(" ");
        Serial.print(4);
        if (inputArray[x] != randomArray[x]) 
        {
          fail();
        }
        x++;
      }
    }
  }
  delay(500);
  turn++; //Increments the turn count, also the last action before starting the output function over again
}

void fail() { //Function used if the player fails to match the sequence
 
  for (int y=0; y<=2; y++)
  { //Flashes lights for failure
    neo(BLANC);
    speakerpin.play(NOTE_G3, 300);
    delay(200);
    neo(NOIR);
    speakerpin.play(NOTE_C3, 300);
    delay(200);
  }
  delay(500);
  turn = -1; //Resets turn value so the game starts over without need for a reset button
}
