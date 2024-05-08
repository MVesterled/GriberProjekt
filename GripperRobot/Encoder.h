#ifndef ENCODER_H
#define ENCODER_H
#include <wiringPi.h>

// Encoder med 20 punkter pr. rotation

class Encoder {
public:
    Encoder();
    void setOrientation(int orientation);
    int getOrientation();
    void updateCounter();   //vigtigt at kalde konstant under aflæsning, ellers ved vi ikke hvor vi er


private:
    volatile int counter;
    volatile int lastCLKState;
	volatile int lastDTState;
    int CLK_PIN = 23; //pin not final
    int DT_PIN = 17; //pin not final
};

#endif
