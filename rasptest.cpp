#include <iostream>
#include <wiringPi.h>
using namespace std;

//g++ -o 

int main()
{
   cout << "Hello pretty!" << endl;
   wiringPiSetupGpio();
   // Set GPIO pin 17 as output
    pinMode(17, OUTPUT);

    // Set GPIO pin 17 high
    digitalWrite(17, HIGH);
    delay(2000);
    digitalWrite(17, LOW);
   
   return 0;
}