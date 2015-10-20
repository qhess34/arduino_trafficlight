// define pin heads
int red = 3;
int orange = 4;
int green = 5;

/* 
- 0 is red
- 1 is orange
- 2 is green
- 99 is all down
*/
int current_light = 99; // all is down by default

/* time variables in ms, don't touch it */
int time = 0;
int lastchange = 0;

/* mod variables 
0 is normal (red, green then orange and etc)
1 is outage (orange blinking)
2 is go_carefully (green blinking)
3 is trains_stop (red blinking)
4 is manual (green or red)
5 is full_manual (green, orange, red)
*/
int mod = 99; // no current mod at this point
int newmod = 0; // default mod loaded during setup function
int delay_default = 1000; // delay for green and red
int delay_orange = 5000; // delay for orange
int delay_blink = 600; // delay for blinking 
bool status = 0; // just for blinking
int current_delay = delay_default;

/* Here we are ! */
void setup() {
	Serial.begin(9600); // open serial connection
	pinMode(red, OUTPUT); // set pin on output
	pinMode(orange, OUTPUT); // set pin on output
	pinMode(green, OUTPUT); // set pin on output
	switchmod(newmod); // load the default mod
}

void loop() {
	// call function off the current mod
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
				digitalWrite(green, HIGH);
				digitalWrite(orange, HIGH);
				digitalWrite(red, LOW);
				current_light = 0;
				break;
			case 1:
			case 2:
			case 3:
				switch (current_light) {
				case 0:
					digitalWrite(red, HIGH);
					break;
				case 1:
					digitalWrite(orange, HIGH);
					break;
				case 2:
					digitalWrite(green, HIGH);
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
}

void full_manual() {
}

/* go to the next light */
int nextlight() {
/*	int current_light;
	int delay;*/
	int delayreturn = 0;

	switch (current_light) {
		case 0:
			digitalWrite(red, HIGH);
			digitalWrite(green, LOW);
			delayreturn = delay_default;
			current_light = 2;
			break;
		case 1:
			digitalWrite(orange, HIGH);
			digitalWrite(red, LOW);
			delayreturn = delay_default;
			current_light = 0;
			break;
		case 2:
			digitalWrite(green, HIGH);			
			digitalWrite(orange, LOW);
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
