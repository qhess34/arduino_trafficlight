/* CAUTION : QUICK & DIRTY ! (new version will be make next time) */
/* This Arduino program allow to control a traffic light (or three light) thanks to a bluetooth app on your mobile */
/* It contains many possibilities (mods) to enjoy your traffic light */
/* I used it with a 4 chanels relay and an Arduino Uno + A Nexus 4 */
/* Author : Quentin HESS */
/* Date of creation : 2015.10.20 */
/* Comments : Still in progress */ 

#include <SoftwareSerial.h>

// define pin heads
int red = 10;
int orange = 9;
int green = 8;
int btrx = 6;
int bttx = 7;

// Create custom serial pin instance
SoftwareSerial mySerial(bttx, btrx); // RX, TX

/*
- 0 is red
- 1 is orange
- 2 is green
- 99 is all down
*/
int current_light = 99; // all is down by default

/* time variables in ms, don't touch it */
long lastchange = 0;

/* mod variables
10 is normal (red, green then orange and etc)
20 is outage (orange blinking)
30 is go_carefully (green blinking)
40 is trains_stop (red blinking)
5X is manual (green (52) or red (50))
6X is full_manual (green (62), orange (61), red (60))
*/
int mod = 99; // technical default mod 
int newmod = 10; // functionnal default mod 
long delay_green = 10000; // delay for green 
long delay_red = 10000; // delay for red
long delay_orange = 3000; // delay for orange
int delay_blink = 600; // delay for blinking
bool status = 0; // just for blinking
long current_delay = delay_red;

/* relays_conf */
bool down  = HIGH;
bool up = LOW;

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

  // Initialize relays status
  digitalWrite(green, down);
  digitalWrite(orange, down);
  digitalWrite(red, down);

  // Load the default mod
  switchmod(newmod); 
}

void loop() {
  // call function off the current mod
  /*Serial.print(digitalRead(red));
  Serial.print(",");
  Serial.print(digitalRead(orange));
  Serial.print(",");
  Serial.print(digitalRead(green));
  Serial.println(" ");*/
 /* delay(1000);*/

  delay(10);
  if (mySerial.available()) { 
	ins=mySerial.read();
	delay(10);
  }
  Serial.print("Receive : ");
  Serial.print(ins);
  Serial.print(" EOC");
  Serial.print("\n");

  updmod=ins;

  switchmod(updmod); 
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
      manual(0);
      break;
    case 52:
      manual(2);
      break;
    case 60:
      full_manual(0);
      break;
    case 61:
      full_manual(1);
      break;
    case 62:
      full_manual(2);
      break;

    default:
      outage();
      break;
   
  }
}

/* function for initializing each mod */
void switchmod(int newmod) {
  if(newmod != mod) {
    switch(newmod) {
      case 10:
        digitalWrite(green, down);
        digitalWrite(orange, down);
        digitalWrite(red, up);
        current_light = 0;
        break;
      case 20:
      case 30:
      case 40:
      case 60:
      case 61:
      case 62:
        switch (current_light) {
        case 0:
          digitalWrite(red, down);
          break;
        case 1:
          digitalWrite(orange, down);
          break;
        case 2:
          digitalWrite(green, down);
          break;
        }
      break;
    }
  mod = newmod;
  Serial.print("Switch to mod : ");
  Serial.print(mod);
  Serial.print("\n");
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
  blinklight(2);
}

void trains_stop() {
  blinklight(0);
}

void outage() {
  blinklight(1);
}

void manual() {
	nextlight();
}

void full_manual(int fmanual_light) {
	switch(fmanual_light) {
		case 0:
			digitalWrite(red, up);
			current_light=0;
			break;
                case 1:
                        digitalWrite(orange, up);
                        current_light=1;
                        break;
                case 2:
                        digitalWrite(green, up);
                        current_light=2;
                        break;
	}			
}

/* go to the next light */
int nextlight() {
/*  int current_light;
  int delay;*/
  long delayreturn = 0;

  switch (current_light) {
    case 0:
      digitalWrite(red, down);
      digitalWrite(green, up);
      delayreturn = delay_green;
      current_light = 2;
      break;
    case 1:
      digitalWrite(orange, down);
      digitalWrite(red, up);
      delayreturn = delay_red;
      current_light = 0;
      break;
    case 2:
      digitalWrite(green, down);     
      digitalWrite(orange, up);
      delayreturn = delay_orange;
      current_light = 1;
      break;
  }    
  return delayreturn;
}

/* blink a light */
void blinklight(int light) {
  if(status) {
    current_light = light;
  }
  else {
    current_light = 99;
  }
 
/*  Serial.print(status);
  Serial.print(",");
  Serial.print(millis());
  Serial.print(",");
  Serial.print(lastchange);
  Serial.print(",");
  Serial.print(light);
  Serial.println(" ");
  Serial.println("======="); */
 
        if(millis() > (lastchange + delay_blink)) {
    lastchange = millis();
    status = !status;
          switch (light) {
                case 0:
                        digitalWrite(red, status);
                        break;
                case 1:
                        digitalWrite(orange, status);
                        break;
                case 2:
                        digitalWrite(green, status);
                        break;
          }
       }
}
