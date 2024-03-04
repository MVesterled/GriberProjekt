#include <iostream>
#include "Pi.h"

int main()
{
    //Make object of Pi, named piCalculator
    Pi piCalculator;

    //Make an int for repeats
    int gentagelser;

    //Output instruction and take input.
    std::cout << "Hvor mange gentagelse skal laves for at finde pi?" << std::endl;
    std::cin >>  gentagelser;

    //Make a double, with the value returned form piCalulator.calcPi, with input gentagelser.
    double piAprox = piCalculator.calcPi(gentagelser);

    //Send the output to terminal.
    std::cout << "Pi er aprox: " << piAprox << " efter " << gentagelser << " gentagelser.";

    return 0;
}
