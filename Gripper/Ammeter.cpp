#include "Ammeter.h"
#include <iostream>

Ammeter::Ammeter(){
    pinMode(CS_SHDNPin,     OUTPUT);
    pinMode(ClockPin,       OUTPUT);
    pinMode(DataOutPin,     OUTPUT);
    pinMode(DataInPin,      INPUT);
}

float Ammeter::readAmmeter() {
    mVolts  = Ammeter::readVoltmeter();
    mAmps   = mVolts / mOhms; //        # ohms law
    return mAmps;
}


// read SPI data from MCP3002 chip
int Ammeter::readVoltmeter() {

  
  // algo
  digitalWrite(CS_SHDNPin, HIGH);

  digitalWrite(ClockPin, LOW); //       # start clock low
  digitalWrite(CS_SHDNPin, LOW); //     # bring CS low

  int commandout = 0; //                # Using channel 0
  commandout |= 0x18; //                # start bit + single-ended bit
  commandout <<= 3; //                  # we only need to send 5 bits here
 
  for (int i=0; i<5; i++) {
    if (commandout & 0x80) 
      digitalWrite(DataOutPin, HIGH);
    else   
      digitalWrite(DataOutPin, LOW);
      
    commandout <<= 1;
    digitalWrite(ClockPin, HIGH);
    digitalWrite(ClockPin, LOW);

  }

  int voltmeterOut = 0;
  // read in one empty bit, one null bit and 10 ADC bits
  for (int i=0; i<12; i++) {
    digitalWrite(ClockPin, HIGH);
    digitalWrite(ClockPin, LOW);
    voltmeterOut <<= 1;
    if (digitalRead(DataInPin))
      voltmeterOut |= 0x1;
  } 
  digitalWrite(CS_SHDNPin, HIGH);

  voltmeterOut >>= 1; //      # first bit is 'null' so drop it
  return voltmeterOut;
}