// Robot avec 4 servos

#define CE_PIN  9
#define CSN_PIN 10
#include <Servo.h>

int pos = 0;    // variable to store the servo position
Servo myservo, myservo1, myservo_rotation , myservo_pince;  // create servo object to control a servo


int up_button     = 2; // Boton Amarillo - A
int down_button   = 4; // Boton Amarillo - C 
int left_button   = 5; // Boton Azul     - D 
int right_button  = 3; // Boton Azul     - B
int start_button  = 6; // Boton F
int select_button = 7; // Boton E
int analog_button = 8; // 
int servo_bras = 9; // 
int servo_bras2 = 10; // 
int servo_rotation = 12; // 
int servo_pince = 13; // 
int x_axis = A0;
int y_axis = A1;
int posH=90;// position horizontale
int posV=90;// " verticale
int buttons[]={up_button, down_button,left_button,
               right_button,start_button,select_button,analog_button};

char msg[20] = "";

void setup(){
    myservo.attach(servo_bras);  // attaches the servo on pin 9 to the servo object
    myservo1.attach(servo_bras2); 
        myservo_rotation .attach(servo_rotation);  
     for(int i; i <7 ; i++)
     {
         pinMode(buttons[i],INPUT);
         digitalWrite(buttons[i],HIGH);  
     } 
     Serial.begin(115200);
  //     sweep();
}
void sweep(){
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
  } 
}

  
void loop(){
// ------------- pince -------------------------
      if(digitalRead(up_button)==LOW)
      {
                    myservo_pince.attach(servo_pince);   
        char msg[]="up";
    //  radio.write(&msg,sizeof(msg));
      delay(300);
        Serial.println("UP Button Pressed");        
        posV += 10;
        if (posV > 180) posV=180;
        myservo_pince.write(posV);  delay(500);
      }
      
       if(digitalRead(down_button)==LOW)
       {
        myservo_pince.attach(servo_pince);  
        char msg[]="down";
 //     radio.write(&msg,sizeof(msg));
     ;
        Serial.println("Down Button Pressed");       
        posV -= 10;
        if (posV < 0) posV=0;
        myservo_pince.write(posV);  delay(500);
      }
// ------------- rotation -------------------------
       if(digitalRead(left_button)==LOW)
      {
        myservo_rotation.attach(servo_rotation);   
        char msg[]="left";
   //   radio.write(&msg,sizeof(msg));
 
        Serial.println("Left Button Pressed");
        posH -= 10;
        if (posH < 0) posV=0; 
        myservo_rotation.write(180 - posH);      delay(300);
      }
       if(digitalRead(right_button)==LOW)
       {
        myservo_rotation.attach(servo_rotation);   
        char msg[]="right";
  //    radio.write(&msg,sizeof(msg));
        Serial.println("Rigth Button Pressed");
        posH += 10;
        if (posH > 180) posH=180;
        myservo_rotation.write(180 - posH); delay(300);
      }
// --------------------------------------
       if(digitalRead(start_button)==LOW)
       {
        char msg[]="start";
  //    radio.write(&msg,sizeof(msg));
      delay(300);
        Serial.println("Start Button Pressed");
      }
       if(digitalRead(select_button)==LOW)
       {
        char msg[]="select";
//      radio.write(&msg,sizeof(msg));
      delay(300);
        Serial.println("Select Button Pressed");
      }
       if(digitalRead(analog_button)==LOW)
       {
        char msg[]="analgobut";
  //    radio.write(&msg,sizeof(msg));
      delay(300);
        Serial.println("Analog Button Pressed");
      }
      Serial.print("\n X = "),Serial.print(analogRead(x_axis)),Serial.print(" \n Y = "), Serial.print(analogRead(y_axis));
      Serial.print("  ");


                      myservo.write(analogRead(y_axis) * 18 / 102); 
                    myservo1.write(analogRead(x_axis) * 18 / 102); 
            delay(300);
                  myservo_pince.detach();  
            myservo_rotation.detach();  
}
