#ifndef AMMETER_H
#define AMMETER_H
#include <wiringPiSPI.h>
#include <iostream>
#define CHANNEL 0
#include <vector>


class AmMeter
{
public:
    AmMeter();

    float getADC(int channel);

private:
      int spi_fd;

};

#endif // AMMETER_H
