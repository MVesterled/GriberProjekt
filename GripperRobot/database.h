#ifndef DATABASE_H
#define DATABASE_H

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <string>


class Database {
public:
    Database(); // Constructor

    void createDatabase(std::string databaseName);

    void useDatabase(std::string database); //Uses database

    void createTableGreb(std::string tableName); //Creates table

    void insertValuesGreb(std::string table, std::string label); //Inserts values

    void printTableGreb(const std::string table); //Prints table

    void deletePointers(); //Deletes pointers

    void createTablePose(std::string tableName); //Creates table

    void insertValuesPose(std::string table, double j1, double j2, double j3, double j4, double j5, double j6); //Inserts values

    void printTablePose(const std::string table); //Prints table

private:
//Creates objects
sql::Driver *driver; // MySQL driver object
sql::Connection *connection; // MySQL connection object
sql::Statement *statement; // MySQL statement object
sql::ResultSet *result; // MySQL result set object


};

#endif
