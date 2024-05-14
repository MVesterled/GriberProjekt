#include "AmMeter.h"

AmMeter::AmMeter()
{
  if ((spi_fd = wiringPiSPISetup(CHANNEL, 1200000)) < 0){
      std::cerr << "Failed to initialize SPI." << std::endl;
}
}

 float AmMeter::getADC(int channel) {
    if (channel != 0) {
        channel = 1;
    }

    std::vector<unsigned char> data(2);
    data[0] = 0b11;
    data[0] = ((data[0] << 1) + channel) << 5;
    data[1] = 0b00000000;

    wiringPiSPIDataRW(CHANNEL, data.data(), data.size());

    unsigned int adc = 0;
    for (int i = 0; i < 2; ++i) {
        adc = (adc << 8) + data[i];
    }
    adc = adc >> 1;

    float voltage = (5 * adc) / 1024.0;

    return voltage;
 }
