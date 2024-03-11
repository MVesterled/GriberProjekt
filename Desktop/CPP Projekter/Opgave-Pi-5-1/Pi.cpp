#include "Pi.h"

double Pi::calcPi(int n) {

    double piResult = 0.0;

    for (int i = 0; i <= n; i++){
        piResult += (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
    }

    return piResult*4;
}

void Pi::setGentagelser(int gentagelser){

}
