//#include <stdlib.h>
#include <iostream>
#include <string>
#include <Database.h>
#include <Motor.h>

int main(void) {
        wiringPiSetupGpio(); //initializer for gpio
        Motor m;
        
        std::cout << "Set speed: (max 512)" << std::endl;
        int tmp=0;
        std::cin >> tmp;
        m.setSpeed(tmp);
        bool dir;
        
        while (true){
        std::cout << "Set direction: (1 = right) (0 = left)" << std::endl;
        std::cin >> dir;
        m.setDirection(dir);
        m.startMotor();
        delay(3000);
        m.stopMotor();
}
        
        
        

/*
    std::cout << "Program startet" << std::endl;
    std::cout << std::endl;
    Database griberData;
    //Motor testObject2;
    //testObject2.printTest2(); 
    std::string db = "test_db";
    std::string table = "force";
    
    std::cout << std::endl;
    griberData.useDatabase(db);
    std::cout << std::endl;
    griberData.createTable(table);
    std::cout << std::endl;
    griberData.insertValues(1, table, "25 Newton");
    std::cout << std::endl;
    griberData.printTable(table);
    std::cout << std::endl;
    griberData.deletePointers();
    std::cout << std::endl;
    std::cout << "Program færdigt" << std::endl;
    * */



    return 0; // Return success
}
