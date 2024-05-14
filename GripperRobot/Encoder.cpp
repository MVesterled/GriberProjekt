#include "Encoder.h"
#include <iostream>

Encoder::Encoder(){
	setOrientation(0);
	volatile int lastCLKState = LOW;
	volatile int lastDTState = LOW;
	
	pinMode(CLK_PIN, INPUT);
	pinMode(DT_PIN, INPUT);
}

int Encoder::getOrientation(){
	return counter;
}

void Encoder::setOrientation(int orientation){
	counter = orientation;
}

void Encoder::updateCounter(){
	int CLKState = digitalRead(CLK_PIN);
	int DTState = digitalRead(DT_PIN);

	if (lastCLKState != CLKState) {
		if (CLKState == HIGH) {
			if (DTState == LOW) {
            counter--;
			} else {
            counter++;
			}
		}
	}
  lastCLKState = CLKState;
  delay(5);
}

