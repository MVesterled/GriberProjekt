#ifndef AMMETER_H
#define AMMETER_H
#include <wiringPi.h>

class Ammeter {
public:
    Ammeter();
    float readAmmeter();
    int readVoltmeter();

private:
    float mAmps;
    int mVolts;
    float mOhms      = 10; // Small resistor
    int ClockPin    = 18; // Not final
    int DataInPin   = 17; // Not final
    int DataOutPin  = 16; // Not final
    int CS_SHDNPin  = 12; // Not final, Chip Select/Shutdown Pin
};

#endif