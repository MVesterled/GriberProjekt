#ifndef DATABASE_H
#define DATABASE_H

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>


class Database {
public:
    Database(); // Constructor
    
    void useDatabase(); //Uses database

    void createTable(); //Creates table

    void insertValues(); //Inserts values

    void printTable(); //Prints table

    void deletePointers(); //Deletes pointers

private:
//Creates objects
sql::Driver *driver; // MySQL driver object
sql::Connection *connection; // MySQL connection object
sql::Statement *statement; // MySQL statement object
sql::ResultSet *result; // MySQL result set object


};

#endif