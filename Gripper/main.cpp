//#include <stdlib.h>
#include <iostream>


#include <Database.h>
#include <Motor.h>

int main(void) {

    std::cout << "Program startet" << std::endl;
    Database griberData;
    //Motor testObject2;
    //testObject2.printTest2();

    griberData.useDatabase();
    griberData.createTable();
    griberData.insertValues();
    griberData.printTable();
    griberData.deletePointers();
    std::cout << "Program færdigt" << std::endl;



    return 0; // Return success
}
