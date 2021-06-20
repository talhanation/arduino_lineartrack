#include <Wire.h>
#include "Arduino.h"
//#include "Keyboard.h"
#include <Adafruit_MotorShield.h>   
/*
#define BAUD (9600) // How fast is the Arduino talking?
#define MAX_BUF (64) // What is the longest message Arduino can store?

char buffer[MAX_BUF]; // where we store the message until we get a ';'
int sofar; // how much is in the buffer   
*/
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);

const int PIN_ONE = 12;
const int PIN_THREE = 13;

int steps; // steps für pos abfrage

static unsigned int stateHoming;
static unsigned int stateInitConsole;

boolean buttonLeft = false;
boolean buttonRight = false;

char consoleInput;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_ONE, INPUT_PULLUP);
  pinMode(PIN_THREE, INPUT_PULLUP);
  stateHoming = 0;
  steps=0; //beim starten immer neues homing machen
  stateInitConsole = 0;
  Serial.println("Linearverfahreinheit gestartet!");
  Serial.println("Bitte t - für steppertest ");
  Serial.println("Bitte h - für homing ");
  Serial.println("Bitte c - für control ");
  Serial.println("Bitte x - für abbrechen ");
  //ready();
  //help;
  
}

void loop() {  
  consoleInput = Serial.read();
  initConsole(consoleInput);
  controlMotor();
}

void initConsole(char consoleInput){
    switch (stateInitConsole){
      case 0:
        //setupMotor(0,false);
        control(false);
        startHoming(false);
        stepperTest(false);
        stateHoming = 0;
        if(consoleInput == 't'){
          Serial.println("StepperTest wurde ausgewählt!");
          stateInitConsole = 1;
        }
        if(consoleInput == 'c'){
          Serial.println("Control wurde ausgewählt!");
          stateInitConsole = 2;
        }
        if(consoleInput == 'h'){
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
        if(consoleInput == 'x'){
          Serial.println("StepperTest wurde abgebrochen!");
          stateInitConsole = 0;
        }

        break;   
      
      case 2:
        control(true);
        startHoming(false);
        stepperTest(false);
        if(consoleInput == 'x'){
          Serial.println("Control wurde abgebrochen!");
          stateInitConsole = 0;
        }
        break;
    
      case 3:
          startHoming(true);
          control(false);
          stepperTest(false);
          if(consoleInput == 'x'){
          Serial.println("Homing wurde abgebrochen!");
          stateInitConsole = 0;
          }
          break;
  }
}

void startHoming(bool start){   
  buttonLeft = digitalRead(PIN_ONE);
  buttonRight = digitalRead(PIN_THREE);

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
        steps++;          
        if(buttonLeft == LOW){
          stateHoming = 3;
          Serial.println("...Homing...  Linker Endstop gefunden... Ende");
        }
        break;
    
        case 3:
          Serial.println("...Homing...Beendet");
          Serial.print("Totale länge der Strecke: ");
          Serial.println(steps);
          myMotor->step(25, RELEASE, DOUBLE);
          stateInitConsole = 0;
          break;
    }
  }
}

void control(bool start){  
  buttonLeft = digitalRead(PIN_ONE);
  buttonRight = digitalRead(PIN_THREE);
  //setupMotor(10);
    
  if (start == true){ 
    if (buttonLeft == LOW) {
      myMotor->step(20, FORWARD, DOUBLE); 
      Serial.println("Linker Stopper Betätigt");
    }
    else if (buttonRight == LOW) {
      myMotor->step(20, BACKWARD, DOUBLE); 
      Serial.println("Rechter Stopper Betätigt");
    }else {
        Serial.println("Loop");
    }  
  }
}

void stepperTest(bool start){ 
  if (start == true){ 
  Serial.println("STEPPER TEST");
  myMotor->step(50, FORWARD, SINGLE); 
  myMotor->step(50, BACKWARD, SINGLE);
  }
}

void controlMotor(){
  switch (stateInitConsole){
      case 0:
        setupMotor(0,false);
      default:
        setupMotor(10, true);
  }
}
void setupMotor(int motorspeed, bool start){
  if (start){
  AFMS.begin();                
  myMotor->setSpeed(motorspeed);  // 10 rpm
  }
}

void getPos(){
  
}

void help() {
  Serial.print(F("Linearverfahreinheit gestartet!"));
  Serial.println(F("Comands:"));
  Serial.println(F("G00 [X(steps)] [F(feedrate)]; - Anfahren steps"));
  Serial.println(F("G01 [X(steps)] [F(feedrate)]; - Anfahren einer Position"));
  Serial.println(F("G04 P[seconds]; - delay"));
  Serial.println(F("G28;  - Homing-Startpunkt"));
  Serial.println(F("G90; - absolute mode"));
  Serial.println(F("G91; - relative mode"));
  Serial.println(F("G92 [X(steps)]; - change logical position"));
  Serial.println(F("M18; - Motor abschalten"));
  Serial.println(F("M100; - Hilfe"));
  Serial.println(F("M114; - Positions- und Geschwindikkeitsabfrage"));
}
/*
void ready() {
  sofar=0; // clear input buffer
  Serial.print(F("> ")); // signal ready to receive i
*/
