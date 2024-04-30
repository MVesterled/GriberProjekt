#ifndef MCP3002_h
#define MCP3002_h

#include <wiringPi.h>

class MCP3002
{
  public:
    MCP3002(int clockpin, int mosipin, int misopin, int cspin);
    int readADC(int adcnum);
  private:
      int _clockpin, _mosipin, _misopin, _cspin;
};


#endif