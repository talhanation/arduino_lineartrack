#include <Wire.h>
#include "Arduino.h"
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

void setup() {
  Serial.begin(9600);
  Serial.println("Bitte t - für steppertest ");
  Serial.println("Bitte h - für homing ");
  Serial.println("Bitte c - für control ");
  Serial.println("Console: ");
  pinMode(PIN_ONE, INPUT_PULLUP);
  pinMode(PIN_THREE, INPUT_PULLUP);
  AFMS.begin();                
  myMotor->setSpeed(10);  // 10 rpm
  stateHoming = 0;
  stateInitConsole = 0;
  buttonLeft = digitalRead(PIN_ONE);
  buttonRight = digitalRead(PIN_THREE);
}

void loop() {  
  initConsole();
  //control(false);
  //startHoming(false);
  //stepperTest(false);
}

void initConsole(){
  if (Serial.available() > 0){
  char consoleInput = char(Serial.read());
  
  switch (stateInitConsole){
    case 0:
      control(false);
      startHoming(false);
      stepperTest(false);
      Serial.println("Bitte t - für steppertest ");
      Serial.println("Bitte h - für homing ");
      Serial.println("Bitte c - für control ");
      if(consoleInput == 't'){
        Serial.println("StepperTest wurde ausgewählt!");
        stateInitConsole = 1;
      }
      else if(consoleInput == 'c'){
        Serial.println("Control wurde ausgewählt!");
        stateInitConsole = 2;
      }
      else if(consoleInput == 'h'){
        Serial.println("Homing wurde ausgewählt!");
        stateInitConsole = 3;
      }
      else{
        Serial.println("Input wurde nicht erkannt!");
        stateInitConsole = 0;
      }
      break;
  
    case 1:
      startHoming(true);      
      if(consoleInput == 'x'){
        Serial.println("StepperTest wurde abgebrochen!");
        stateInitConsole = 0;
      }   
      break;  
    
    case 2:
      control(true);
      if(consoleInput == 'x'){
        Serial.println("Control wurde abgebrochen!");
        stateInitConsole = 0;
      }
      break;

    case 3:
        startHoming(true);
        if(consoleInput == 'x'){
        Serial.println("Homing wurde abgebrochen!");
        stateInitConsole = 0;
        }
        break;
    
    
    /*if (input =='r'){
      //r_waspressed = true;
      Serial.println("SIGNAL");
      digitalWrite(8, HIGH);
      
    }
    else if (input == 'x'){
    Serial.println("NO SIGNAL");
    //STOP = true;
    }
    */
    }
  }
}

void startHoming(bool start){  
  
  //buttonLeft = digitalRead(PIN_ONE);
  //buttonRight = digitalRead(PIN_THREE);

  if (start){  
    switch (stateHoming)
    {
      case 0:
          myMotor->step(0, FORWARD, DOUBLE);
          myMotor->step(0, BACKWARD, DOUBLE); 
          delay(100);
          Serial.print("...Homing...Gestartet...");
          stateHoming = 1;
          break;
       
       case 1:
          myMotor->step(10, FORWARD, DOUBLE);
          Serial.println("...Homing...  Suche nach rechtem Endstop");
          
          if(buttonRight == LOW){
            stateHoming = 2;
            Serial.println("...Homing...  Rechter Endstop gefunden");
          }
          break;
       
       case 2:
        myMotor->step(10, BACKWARD, DOUBLE);  
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
}

void control(bool start){  
  //buttonLeft = digitalRead(PIN_ONE);
  //buttonRight = digitalRead(PIN_THREE);
  
  if (start){ 
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

  if (start){ 
  Serial.println("Double coil steps");
  myMotor->step(100, FORWARD, DOUBLE); 
  myMotor->step(100, BACKWARD, DOUBLE);
  }
}

double setPos(){
  
}

void getPos(){
  
}
