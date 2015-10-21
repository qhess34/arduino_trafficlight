/* This Arduino program allow to control a traffic light (or three light) thanks to a bluetooth app on your mobile */
/* It contains many possibilities (mods) to enjoy your traffic light */
/* I used it with a 4 chanels relay and an Arduino Uno + A Nexus 4 */
/* Author : Quentin HESS */
/* Date of creation : 2015.10.20 */
/* Comments : Still in progress (I'm waiting the bluetooth module for Arduino) */

// define pin heads
int red = 10;
int orange = 9;
int green = 8;

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
0 is normal (red, green then orange and etc)
1 is outage (orange blinking)
2 is go_carefully (green blinking)
3 is trains_stop (red blinking)
4 is manual (green or red)
5 is full_manual (green, orange, red)
*/
int mod = 99; // no current mod at this point
int newmod = 1; // default mod loaded during setup function
long delay_default = 10000; // delay for green and red
long delay_orange = 3000; // delay for orange
int delay_blink = 600; // delay for blinking 
bool status = 0; // just for blinking
long current_delay = delay_default;

/* relays_conf */
bool down  = HIGH;
bool up = LOW;

/* Here we are ! */
void setup() {
  Serial.begin(9600); // open serial connection
  pinMode(red, OUTPUT); // set pin on output
  pinMode(orange, OUTPUT); // set pin on output
  pinMode(green, OUTPUT); // set pin on output
  digitalWrite(green, down);
  digitalWrite(orange, down);
  digitalWrite(red, down);
  switchmod(newmod); // load the default mod
}

void loop() {
  // call function off the current mod
  Serial.print(digitalRead(red));
  Serial.print(",");
  Serial.print(digitalRead(orange));
  Serial.print(",");
  Serial.print(digitalRead(green));
  Serial.println(" ");
  delay(1000);
  
  switch (mod) {
    case 0:
      normal();
      break;
    case 2:
      go_carefully();
      break;
    case 3:
      trains_stop();
      break;
    case 4:
      manual();
      break;
    case 5:
      full_manual();
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
      case 0:
        digitalWrite(green, down);
        digitalWrite(orange, down);
        digitalWrite(red, up);
        current_light = 0;
        break;
      case 1:
      case 2:
      case 3:
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
/* WIP */
}

void full_manual() {
/* WIP */
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
      delayreturn = delay_default;
      current_light = 2;
      break;
    case 1:
      digitalWrite(orange, down);
      digitalWrite(red, up);
      delayreturn = delay_default;
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
  
  Serial.print(status);
  Serial.print(",");
  Serial.print(millis());
  Serial.print(",");
  Serial.print(lastchange);
  Serial.print(",");
  Serial.print(light);
  Serial.println(" ");
  Serial.println("=======");
  
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

