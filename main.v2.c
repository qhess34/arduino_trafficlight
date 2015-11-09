/* CAUTION : QUICK & DIRTY ! */
/* This Arduino program allow to control a traffic light (or three light) thanks to a bluetooth app on your mobile */
/* It contains many possibilities (mods) to enjoy your traffic light */
/* I used it with a 4 chanels relay and an Arduino Uno + A Nexus 4 */
/* Author : Quentin HESS */
/* Date of creation : 2015.10.20 */
/* Comments : Still in progress */ 

#include <SoftwareSerial.h>

/* CONFIGURATION */
// define pin heads
const int red = 10;
const int orange = 9;
const int green = 8;
const int btrx = 6;
const int bttx = 7;

/* mode variables
10 is normal (red, green then orange and etc)
20 is outage (orange blinking)
30 is go_carefully (green blinking)
40 is trains_stop (red blinking)
5X is manual (green (52) or red (50))
6X is full_manual (green (62), orange (61), red (60))
7X delays (reduce/rise green: 70/71, orange: 72/73, red: 74/75, blink: 76/77))
*/
int newmod = 10; // functionnal default mod 

/* Delays */
long delay_green = 10000; // delay for green 
long delay_red = 10000; // delay for red
long delay_orange = 3000; // delay for orange
int delay_blink = 600; // delay for blinking

/* relays_conf */
const bool down  = HIGH;
const bool up = LOW;

/* END CONFIGURATION */

int current_light = 99; // all is down by default
long lastchange = 0; // time variables in ms, don't touch it 
int mod = 99; // technical default mod 
bool binblink = 0; // just for blinking
long current_delay = delay_red;
int ins;

// Create custom serial pin instance
SoftwareSerial mySerial(bttx, btrx); // RX, TX

/* Here we are ! */
void setup() {

  // Initialize Serial Connection for debug
  Serial.begin(9600); // open serial connection
  Serial.println("Start of TL Arduino");

  // Initialize BT Connection for instructions
  mySerial.begin(9600);
  mySerial.println("Open BT connection");

  // Initialize relay pins
  pinMode(red, OUTPUT); // set pin on output
  pinMode(orange, OUTPUT); // set pin on output
  pinMode(green, OUTPUT); // set pin on output

  // Initialize relays 
  digitalWrite(green, down);
  digitalWrite(orange, down);
  digitalWrite(red, down);

  // Load the default mode
  switchmod(newmod); 
}

void loop() {
  // call function off the current mod
  Serial.print(current_light);
  Serial.print(",");
  Serial.print(digitalRead(red));
  Serial.print(",");
  Serial.print(digitalRead(orange));
  Serial.print(",");
  Serial.print(digitalRead(green));
  Serial.print("\n");

  if (mySerial.available()) { 
	  ins=(int) mySerial.read();
	  Serial.print("Receive : ");
	  Serial.print(ins);
	  Serial.print(" EOC");
	  Serial.print("\n");

	  if(ins == 10 || ins == 20 || ins == 30 || ins == 40 || ins == 50 || ins == 52 || ins == 60 || ins == 61 || ins == 62) {
	    newmod = ins;
	  }
	  else if(ins == 70 && delay_green >= 2000) {
		delay_green -= 1000;
	  }
          else if(ins == 71 && delay_green < 1000000) {
                delay_green += 1000;
          }
          else if(ins == 72 && delay_orange >= 2000) {
                delay_orange -= 1000;
          }
          else if(ins == 73 && delay_orange < 1000000) {
                delay_orange += 1000;
          }
          else if(ins == 74 && delay_red >= 2000) {
                delay_red -= 1000;
          }
          else if(ins == 75 && delay_red < 1000000) {
                delay_red += 1000;
          }
          else if(ins == 76 && delay_blink >= 500) {
                delay_blink -= 200;
          }
          else if(ins == 77 && delay_blink < 10000) {
                delay_blink += 200;
          }

	
  }

  switchmod(newmod); 
  switch (mod) {
    case 10:
      normal();
      break;
    case 30:
      go_carefully();
      break;
    case 40:
      trains_stop();
      break;
    case 50:
      manual(red);
      break;
    case 52:
      manual(green);
      break;
    case 60:
      full_manual(red);
      break;
    case 61:
      full_manual(orange);
      break;
    case 62:
      full_manual(green);
      break;
    default:
      outage();
      break;
   
  }
  delay(100);
}

/* function for initializing each mod */
void switchmod(int newmod) {
  if(newmod != mod) {
      mod = newmod;
      Serial.print("Switch to mod : ");
      Serial.print(mod);
      Serial.print("\n");
  
    switch(newmod) {
      case 10:
	changeto(red);
        break;
      case 20:
      case 30:
      case 40:
      case 60:
      case 61:
      case 62:
	shutdownall();
        break;
      case 50:
      case 52:
      	current_delay = 0;
      break;
    }

  }
}

/* list off mods */
void normal() {
  if(millis() > (lastchange + current_delay)) {
    lastchange = millis();
    current_delay = nextlight();
  }
  /*Serial.println(lastchange);
  Serial.println(current_delay);
  Serial.println("===");*/
}

void go_carefully() {
  blinklight(green);
}

void trains_stop() {
  blinklight(red);
}

void outage() {
  blinklight(orange);
}

void manual(int manual_light) {
  if(current_light != manual_light) {
    if(millis() > (lastchange + current_delay)) {
      lastchange = millis();
      current_delay = nextlight();
    }
}
}

void full_manual(int fmanual_light) {
	changeto(fmanual_light);
}

/* go to the next light */
int nextlight() {
  long delayreturn = 0;

  switch (current_light) {
    case red:
	changeto(green);
	delayreturn = delay_green;
        break;
    case orange:
	changeto(red);
        delayreturn = delay_red;
        break;
    case green:
	changeto(orange);
      	delayreturn = delay_orange;
        break;
  }    
  return delayreturn;
}

/* blink a light */
void blinklight(int light) {

  if(binblink) {
    current_light = light;
  }
  else {
    current_light = 99;
  }

  if(millis() > (lastchange + delay_blink)) {
    lastchange = millis();
    binblink = !binblink;
    digitalWrite(light, binblink);
  }
}

void shutdownall() {
	
	if(Serial.print(digitalRead(red))) { digitalWrite(red, down); }
	if(Serial.print(digitalRead(green))) { digitalWrite(green, down); }
	if(Serial.print(digitalRead(orange))) { digitalWrite(orange, down); }
	current_light = 99;
		
}

void switchon(int light) {
	digitalWrite(light, up);
}

void changeto(int light) {
	if(light != current_light) {
		shutdownall();
		switchon(light);
		current_light=light;
	}

}
