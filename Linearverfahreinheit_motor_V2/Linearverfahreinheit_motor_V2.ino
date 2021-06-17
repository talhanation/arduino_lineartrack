#include <Wire.h>
#include <Console.h>
#include "Arduino.h"
#include <Adafruit_MotorShield.h>      

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);

const int PIN_ONE = 12;
const int PIN_THREE = 13;

static unsigned int stateHoming;

boolean buttonLeft = false;
boolean buttonRight = false;

void setup() {
  pinMode(PIN_ONE, INPUT_PULLUP);
  pinMode(PIN_THREE, INPUT_PULLUP);
  Serial.begin(9600);
  AFMS.begin();                
  myMotor->setSpeed(50);  // 10 rpm
  stateHoming = 0;
  Bridge.begin();
  Console.begin();
  while (!Console);
  Console.println("Hi, what's your name?");
}

void loop() {  
  initConsole(); 
  control(true);
  startHoming(false);
}

void initConsol(){
  
  if (Console.available() > 0) {

    char c = Console.read();
}

void startHoming(bool start){  
  
  buttonLeft = digitalRead(PIN_ONE);
  buttonRight = digitalRead(PIN_THREE);
  switch (stateHoming)
  {
    case 0:
        myMotor->step(0, FORWARD, DOUBLE);
        myMotor->step(0, BACKWARD, DOUBLE); 
        Serial.print("...Homing...Gestartet...");
        delay(100);
        if(start == true){
          stateHoming = 1;
        }
        break;
     
     case 1:
        myMotor->step(1, FORWARD, DOUBLE);
        Serial.println("...Homing...  Suche nach rechtem Endstop");
        
        if(buttonRight == LOW){
          stateHoming = 2;
          Serial.println("...Homing...  Rechter Endstop gefunden");
        }
        break;
     
     case 2:
      myMotor->step(1, BACKWARD, DOUBLE);  
      Serial.println("...Homing...  Suche nach linkem Endstop");
                
      if(buttonLeft == LOW){
        stateHoming = 3;
        Serial.println("...Homing...  Linker Endstop gefunden... Ende");
      }
      break;

      case 3:
        Serial.println("...Homing...Beendet");
        myMotor->step(0, FORWARD, DOUBLE);
        myMotor->step(0, BACKWARD, DOUBLE); 
        break;
  }
  
}

void control(bool start){  
  buttonLeft = digitalRead(PIN_ONE);
  buttonRight = digitalRead(PIN_THREE);
  
  if (start){ 
    if (buttonRight == LOW) {
      myMotor->step(1, FORWARD, DOUBLE); 
      Serial.println("Rechter Stopper Betätigt");
    }
    else if (buttonLeft == LOW) {
      myMotor->step(1, BACKWARD, DOUBLE); 
      Serial.println("Linker Stopper Betätigt");
    }else {
        Serial.println("Loop");
        myMotor->step(0, FORWARD, DOUBLE);
        myMotor->step(0, BACKWARD, DOUBLE); 
    }  
  }
}

double setPos(){
  
}

void getPos(){
  
}
