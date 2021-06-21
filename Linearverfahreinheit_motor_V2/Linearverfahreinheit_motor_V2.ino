#include <Wire.h>
#include "Arduino.h"
//#include "Keyboard.h"
#include <Adafruit_MotorShield.h>   
#define BAUD (9600)
/*
#define MAX_BUF (64) // What is the longest message Arduino can store?

char buffer[MAX_BUF]; // where we store the message until we get a ';'
int sofar; // how much is in the buffer   
*/
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);

const int PIN_ONE = 12;
const int PIN_THREE = 13;

float POS;

float input_l_in_mm = 0;
float input_pos_in_mm = 0;
float addSteps = 0; 
int steps = 0; // steps für pos abfrage

static unsigned int stateHoming;
static unsigned int stateInitConsole;
static unsigned int stateAddPos;
static unsigned int stateSetPos;

boolean buttonLeft = false;
boolean buttonRight = false;

boolean stopMotor = false;

char consoleInput;
float x, dx;

void setup() {
  Serial.println("Linearverfahreinheit V1 gestartet!");
  Serial.begin(BAUD);
  pinMode(PIN_ONE, INPUT_PULLUP);
  pinMode(PIN_THREE, INPUT_PULLUP);
  stateHoming = 0;
  stateAddPos = 0;
  stateSetPos = 0;
  
  steps = 0; //beim starten immer neues homing machen
  stateInitConsole = 0;
  help();
  //setupMotor(15, true); //hard mode - motor immer aktiv - controlMotor muss ausgeklammert werden
  
}

void help(){
  Serial.println("-------------------------------------------------");
  Serial.println("Bitte t - für Steppertest. ");
  Serial.println("Bitte h - für Homing. ");
  Serial.println("Bitte c - für Control mit den Endstopps.");
  Serial.println("Bitte x - für Abbrechen. ");
  Serial.println("Bitte a - für Positionsabfrage. (erst Homing machen)");
  Serial.println("Bitte l - für Anfahren für eine bestimme Länge. ");
  Serial.println("Bitte k - für Anfahren einer bestimmen Position. ");
  Serial.println("-------------------------------------------------");
}

void loop() {  
  consoleInput = Serial.read();
  initConsole(consoleInput);
  controlMotor(true); //safe mode - motor aus im leerlauf --> harken beim homing und control
  //GCodeReader();?????????????
}

int intInput(){
  
  char str[10];
  int charcount=0;
  memset(str,0,sizeof(str)); // String-Puffer löschen
  while (!Serial.available()); // Warten bis 1. Zeichen im Eingangspuffer
  delay(100); // Warten auf weitere Zeichen im Eigangspuffer
  while (Serial.available() && charcount<9)
  {
    str[charcount]=Serial.read(); // Zeichen aus Eingangspuffer lesen
    charcount++;
  }
  return atoi(str); // String in Integer-Rückgabewert wandeln
}

void controlMotor(bool start){
  buttonLeft = digitalRead(PIN_ONE);
  buttonRight = digitalRead(PIN_THREE);
  if (start == true){
    switch (stateInitConsole){
        case 0:
          setupMotor(0,false);
        default:
          if ((stateInitConsole == 5) || (stateInitConsole == 6)) {
              if ((buttonLeft == LOW)||(buttonLeft == LOW)){
                releaseMotor();
              }else {
                setupMotor(15, true);
              }
          }else {
          setupMotor(15, true);
          }
    }
  }
}

void releaseMotor(){
  myMotor->step(15, RELEASE, DOUBLE);
}

void initConsole(char consoleInput){
    switch (stateInitConsole){
      case 0:
        control(false);
        startHoming(false);
        stepperTest(false);
        addPos(false);
        setPos(false);
        stateHoming = 0;
        stateAddPos = 0;
        stateSetPos = 0;
        
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
        if(consoleInput == 'a'){
          stateInitConsole = 4;
        }
        if(consoleInput == 'l'){
          Serial.println("addPos wurde ausgewählt!");
          stateInitConsole = 5;
        }
        if(consoleInput == 'k'){
          Serial.println("Anfahren zu Position wurde ausgewählt!");
          stateInitConsole = 6;
        }
        break;
        
      case 1:
        stepperTest(true);  
        startHoming(false);   
        control(false);
        addPos(false);
        setPos(false);
        if(consoleInput == 'x'){
          Serial.println("StepperTest wurde abgebrochen!");
          stateInitConsole = 0;
        }

        break;   
    
      case 2:
        control(true);
        startHoming(false);
        stepperTest(false);
        addPos(false);
        setPos(false);
        if(consoleInput == 'x'){
          Serial.println("Control wurde abgebrochen!");
          stateInitConsole = 0;
        }
        break;
    
      case 3:
          startHoming(true);
          control(false);
          stepperTest(false);
          addPos(false);
          setPos(false);
          if(consoleInput == 'x'){
            Serial.println("Homing wurde abgebrochen!");
            stateInitConsole = 0;
          }
          break;

     case 4:
          startHoming(false);
          control(false);
          stepperTest(false);
          addPos(false);
          setPos(false);
          getPos();
          if(consoleInput == 'x'){
            Serial.println("Positionsabfrage wurde abgebrochen!");
            stateInitConsole = 0;
          }
          stateInitConsole = 0;
          break;     
     
     case 5:
          startHoming(false);
          control(false);
          stepperTest(false);
          setPos(false);
          addPos(true);
          if(consoleInput == 'x'){
            Serial.println("Anfahren für eine Länge wurde abgebrochen!");
            stateInitConsole = 0;
          }
          break;
     case 6:    
          startHoming(false);
          control(false);
          stepperTest(false);
          addPos(false);
          setPos(true);
          if(consoleInput == 'x'){
            Serial.println("Anfahren für eine Länge wurde abgebrochen!");
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
          myMotor->step(5, BACKWARD, DOUBLE);
          Serial.println("...Homing...  Suche nach rechtem Endstop");
          
          if(buttonRight == LOW){
            stateHoming = 2;
            Serial.println("...Homing...  Rechter Endstop gefunden");
          }
          break;
       
       case 2:
        myMotor->step(5, FORWARD, DOUBLE);  
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
          Serial.println(steps * 5);
          
          releaseMotor();
          POS = 0;
          help();
          stateInitConsole = 0;
          break;
    }
  }
}

void control(bool start){  
  buttonLeft = digitalRead(PIN_ONE);
  buttonRight = digitalRead(PIN_THREE);
    
  if (start == true){ 
    if (buttonLeft == LOW) {
      POS += 2.87;
      myMotor->step(10, FORWARD, DOUBLE); 
      Serial.println("Linker Stopper Betätigt");
    }
    else if (buttonRight == LOW) {
      POS += 2.87;
      myMotor->step(10, BACKWARD, DOUBLE); 
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

void setupMotor(int motorspeed, bool start){
  if (start){
  AFMS.begin();                
  myMotor->setSpeed(motorspeed);  // 10 rpm
  }
}

float getPos(){
  Serial.print("Aktuelle Postition: ");
  Serial.print(POS);
  Serial.println(" mm");
  return POS;
}

void setPos(bool start){
  
   if (start == true){
      
    switch(stateSetPos){
      
      case 0:
            input_pos_in_mm = 0;
            addSteps = 0; 
            Serial.println("Bitte die Position in mm angeben: " );
            delay(200);
            input_pos_in_mm = intInput();
            Serial.print("Ausgewählte Position ist: ");
            Serial.print(input_pos_in_mm);
            Serial.println(" mm");
            x = getPos();
            dx = (input_pos_in_mm - x);
                      
            if (dx > 0){
              Serial.print("Differenz zur Position ist: ");
              Serial.print(dx);
              Serial.println(" mm");
              Serial.print("Es wird zu ");
              Serial.print(input_pos_in_mm);
              Serial.print(" mm ");
              Serial.println("Angefahren");
              stateSetPos = 1;
            }
            else if (dx < 0){
              Serial.print("Differenz zur Position ist: ");
              Serial.print(dx * (-1));
              Serial.println(" mm");
              Serial.print("Es wird zu ");
              Serial.print(input_pos_in_mm);
              Serial.print(" mm ");
              Serial.println("Angefahren");
              stateSetPos = 2;
            }else {
              Serial.println("Länge ungültig! " );
              stateSetPos = 0;
            }
            break;  
      case 1:
            addSteps = 2.87 * dx;
            Serial.println(addSteps);
            myMotor->step(addSteps, BACKWARD, DOUBLE);
            POS += dx;
            help();
            stateInitConsole = 0;
            break;

      case 2:
            addSteps = 2.87 * dx * (-1);
            Serial.println(addSteps);
            myMotor->step(addSteps, FORWARD, DOUBLE);
            POS -= (dx * (-1));
            help();
            stateInitConsole = 0;
            break;
      }
    }
}

void addPos(bool start){
  buttonLeft = digitalRead(PIN_ONE);
  buttonRight = digitalRead(PIN_THREE);
  if (start == true){  
    
    switch(stateAddPos){
      
      case 0:
            input_l_in_mm = 0;
            addSteps = 0; 
            Serial.println("Bitte die Länge in mm angeben: " );
            delay(200);
            input_l_in_mm = intInput();
            Serial.print("Ausgewählte länge ist: ");
            Serial.print(input_l_in_mm);
            Serial.println(" mm");
            if (input_l_in_mm > 0){
              Serial.print("Es wird ");
              Serial.print(input_l_in_mm);
              Serial.print(" mm ");
              Serial.println("lang gefahren");
              stateAddPos = 1;
            } 
            else if (input_l_in_mm < 0){
              Serial.print("Es wird ");
              Serial.print(input_l_in_mm *(-1));
              Serial.print(" mm ");
              Serial.println("lang gefahren");
              stateAddPos = 2;
            }
            else {
              Serial.println("Position ungültig! " );
              stateAddPos = 0;
            }
            break;
      
      case 1:
            
            addSteps = input_l_in_mm * 2.87;
            Serial.println(addSteps);
            myMotor->step(addSteps, BACKWARD, DOUBLE);
            POS += input_l_in_mm; 
            help();
            stateInitConsole = 0;
            //stateAddPos = 0;
            break;
      
      case 2:          
            float addSteps = input_l_in_mm * 2.87 *(-1); // 1 mm = 2,174 steps
            Serial.println(addSteps);
            myMotor->step(addSteps, FORWARD, DOUBLE);
            POS -= (input_l_in_mm * (-1));
            help();
            stateInitConsole = 0;
            //stateAddPos = 0;
            break;
            
    }
  }
}
/*
void help() {
  Serial.println(F("Linearverfahreinheit gestartet!"));
  Serial.println(F("Comands:"));
  Serial.println(F("G00 [X(steps)] [F(feedrate)]; - Anfahren steps"));
  Serial.println(F("G01 [X(steps)] [F(feedrate)]; - Anfahren einer Position"));
  Serial.println(F("G04 P[seconds]; - delay"));
  Serial.println(F("G28;  - Homing-Startpunkt"));
  Serial.println(F("M18; - Motor abschalten"));
  Serial.println(F("M100; - Hilfe"));
  Serial.println(F("M114; - Positions- und Geschwindikkeitsabfrage"));
}
/*
void CmdReady() {
  sofar=0; // clear input buffer
  Serial.print("> "); // signal ready to receive i
}

void GCodeReader(){

    char c = Serial.read(); // get it
    Serial.print(c); // optional: repeat back what I got for debugging

    // store the byte as long as there's room in the buffer.
    // if the buffer is full some data might get lost
    if(sofar < MAX_BUF) buffer[sofar++]=c;
    // if we got a return character (\n) the message is done.
    if(c=='\n') {
      Serial.print(F("\r\n")); // optional: send back a return for debugging

      // strings must end with a \0.
      buffer[sofar]=0;
      processCommand(); // do something with the command
      CmdReady();
    }
}

/**
 * Read the input buffer and find any recognized commands. One G or M command per line.
 */
/*
void processCommand() {
  // look for commands that start with 'G'
  int cmd=parsenumber('G',-1);
  switch(cmd) {
  case 0: // move in a line
  case 1: // move in a line
    //setFeedRate(parsenumber('F',fr));
    //line( parsenumber('X',(mode_abs?px:0)) + (mode_abs?0:px),
    //parsenumber('Y',(mode_abs?py:0)) + (mode_abs?0:py) );
    break;
  case 4: //pause(parsenumber('P',0)*1000); break; // wait a while
  case 92: // set logical position
    position( parsenumber('X',0));
    break;
  default: break;
  }

  // look for commands that start with 'M'
  cmd=parsenumber('M',-1);
  switch(cmd) {
  case 18: // turns off power to steppers (releases the grip)
    setupMotor(0, false);
    break;
    
  case 100: help(); break;
  
  case 114: getPos(); break; // prints px, py, fr, and mode.
  default: break;
  }

  // if the string has no G or M commands it will get here and the Arduino will silently ignore it
}

float parsenumber(char code,float val) {
  char *ptr=buffer;
  
  while(ptr && *ptr && ptr<buffer+sofar) {
      if(*ptr==code) {
        return atof(ptr+1);
      }
    ptr=strchr(ptr,' ')+1;
    }
  return val;
}

void position(float npx) {
  px=npx;
}
*/
