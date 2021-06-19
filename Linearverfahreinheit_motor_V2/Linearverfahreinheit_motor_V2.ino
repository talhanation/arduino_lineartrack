#include <Wire.h>
#include "Arduino.h"
#include "Keyboard.h"
#include <Adafruit_MotorShield.h>      

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);

const int PIN_ONE = 12;
const int PIN_THREE = 13;

static unsigned int stateHoming;
static unsigned int stateInitConsole;

boolean buttonLeft = false;
boolean buttonRight = false;
boolean STOP = false;
boolean homingstart = false;
boolean input = false;

String consoleInput;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_ONE, INPUT_PULLUP);
  pinMode(PIN_THREE, INPUT_PULLUP);
  stateHoming = 0;
  stateInitConsole = 0;
  Serial.println("Linearverfahreinheit gestartet!");
  Serial.println("Bitte t - für steppertest ");
  Serial.println("Bitte h - für homing ");
  Serial.println("Bitte c - für control ");
  Serial.println("Bitte x - für abbrechen ");
  Serial.println("Console: ");
}

void loop() {  
  consoleInput = Serial.read();
  initConsole(consoleInput);
}

void initConsole(String consoleInput){
    switch (stateInitConsole){
      case 0:
        control(false);
        startHoming(false);
        stepperTest(false);
        stateHoming = 0;
        if(consoleInput == "t"){
          Serial.println("StepperTest wurde ausgewählt!");
          stateInitConsole = 1;
        }
        if(consoleInput == "c"){
          Serial.println("Control wurde ausgewählt!");
          stateInitConsole = 2;
        }
        if(consoleInput == "G28"){
          Serial.println("Homing wurde ausgewählt!");
          stateInitConsole = 3;
        }
        else{
        }
        break;
        
      case 1:
        stepperTest(true);  
        startHoming(false);   
        control(false); 
        if(consoleInput == "x"){
          Serial.println("StepperTest wurde abgebrochen!");
          stateInitConsole = 0;
        }

        break;   
      
      case 2:
        control(true);
        startHoming(false);
        stepperTest(false);
        if(consoleInput == "x"){
          Serial.println("Control wurde abgebrochen!");
          stateInitConsole = 0;
        }
        break;
    
      case 3:
          startHoming(true);
          control(false);
          stepperTest(false);
          if(consoleInput == "x"){
          Serial.println("Homing wurde abgebrochen!");
          stateInitConsole = 0;
          }
          break;
  }
}

void startHoming(bool start){  
  
  buttonLeft = digitalRead(PIN_ONE);
  buttonRight = digitalRead(PIN_THREE);
  setupMotor(10);
  
  if (start == true){  
    switch (stateHoming)
    {
      case 0: 
          Serial.println("...Homing...Gestartet...");
          stateHoming = 1;
          break;
       
       case 1:
          myMotor->step(10, BACKWARD, DOUBLE);
          Serial.println("...Homing...  Suche nach rechtem Endstop");
          
          if(buttonRight == LOW){
            stateHoming = 2;
            Serial.println("...Homing...  Rechter Endstop gefunden");
          }
          break;
       
       case 2:
        myMotor->step(10, FORWARD, DOUBLE);  
        Serial.println("...Homing...  Suche nach linkem Endstop");
                  
        if(buttonLeft == LOW){
          stateHoming = 3;
          Serial.println("...Homing...  Linker Endstop gefunden... Ende");
        }
        break;
    
        case 3:
          Serial.println("...Homing...Beendet");
          myMotor->step(100,RELEASE, DOUBLE);
          stateInitConsole = 0;
          break;
    }
  }
}

void control(bool start){  
  buttonLeft = digitalRead(PIN_ONE);
  buttonRight = digitalRead(PIN_THREE);
  setupMotor(10);
    
  if (start == true){ 
    if (buttonLeft == LOW) {
      myMotor->step(10, FORWARD, DOUBLE); 
      Serial.println("Linker Stopper Betätigt");
    }
    else if (buttonRight == LOW) {
      myMotor->step(10, BACKWARD, DOUBLE); 
      Serial.println("Rechter Stopper Betätigt");
    }else {
        Serial.println("Loop");
    }  
  }
}

void stepperTest(bool start){
  setupMotor(10);
  
  if (start == true){ 
  Serial.println("STEPPER TEST");
  myMotor->step(100, FORWARD, SINGLE); 
  myMotor->step(100, BACKWARD, SINGLE);
  }
}

void setupMotor(int motorspeed){
  AFMS.begin();                
  myMotor->setSpeed(motorspeed);  // 10 rpm
}

double setPos(){
  
}

void getPos(){
  
}
