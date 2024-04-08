//#include <stdlib.h>
#include <iostream>
#include <string>

#include <Database.h>
#include <Motor.h>

int main(void) {

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



    return 0; // Return success
}
