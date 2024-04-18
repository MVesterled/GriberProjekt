#include "database.h"
#include <iostream>

// -> bruges for at indikere at vi bruger en memberfunktion i vores pointerobjekter

//Constructor that makes connection to the database
Database::Database() {
    std::cout << "Prøver at lave forbindelse..." << std::endl;
    try {
        /* Create a connection */
        driver = get_driver_instance(); // Get MySQL driver instance
        connection = driver->connect("tcp://127.0.0.1:3306", "root", "root"); // Connect to MySQL server (IP Address, user name, password)
        statement = connection->createStatement(); // Create a MySQL statement object
        std::cout << "Forbindelse OK" << std::endl;
    }
        catch (sql::SQLException &e) { // Catch MySQL exceptions
            std::cout << "# ERR: " << e.what(); // Print error message
            std::cout << " (MySQL error code: " << e.getErrorCode(); // Print MySQL error code
            std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl; // Print SQL state
            }
}

//Function that create a database
    void Database::createDatabase(std::string databaseName){
        try {
            //std::string dropQuery = "DROP DATABASE IF EXISTS " + databaseName;
            //statement->execute(dropQuery); // Drops database if exist already
            std::string createQuery = "CREATE DATABASE " + databaseName;
            statement->execute(createQuery); // Creates database with given name
            std::cout << "Database oprettet" << std::endl;
        }
            catch (sql::SQLException &e) { // Catch MySQL exceptions
                std::cout << "# ERR: " << e.what(); // Print error message
                std::cout << " (MySQL error code: " << e.getErrorCode(); // Print MySQL error code
                std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl; // Print SQL state
            }
    }

//Function that decides witch database we are using
    void Database::useDatabase(std::string database){
        try {
            std::string query = "USE " + database;
            statement->execute(query); // Set specified database as the current database
            std::cout << "Database i brug" << std::endl;
        }
            catch (sql::SQLException &e) { // Catch MySQL exceptions
                std::cout << "# ERR: " << e.what(); // Print error message
                std::cout << " (MySQL error code: " << e.getErrorCode(); // Print MySQL error code
                std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl; // Print SQL state
            }
    }

//Function that creates a table in the active database
    void Database::createTable(std::string tableName){
        try {
            std::string dropQuery = "DROP TABLE IF EXISTS `" + tableName + "`"; // Drop 'tableName' table if it exists
            statement->execute(dropQuery);

            std::string createQuery = "CREATE TABLE `" + tableName + "` (id INT, label CHAR(25))"; // Create 'tableName' table with id (INT) and label (CHAR(25))
            statement->execute(createQuery);

        std::cout << "Table created" << std::endl;
        }
        catch (sql::SQLException &e) { // Catch MySQL exceptions
            std::cout << "# ERR: " << e.what(); // Print error message
            std::cout << " (MySQL error code: " << e.getErrorCode(); // Print MySQL error code
            std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl; // Print SQL state
            }
    }

//Function that inserts values into table
    void Database::insertValues(int id, std::string table, std::string label){
        try {
            std::string insertQuery = "INSERT INTO `" + table + "` (id, label) VALUES (" + std::to_string(id) + ", '" + label + "')"; // Insert values into table
        statement->execute(insertQuery);
        std::cout << "Values inserted into table" << std::endl;
        }
        catch (sql::SQLException &e) { // Catch MySQL exceptions
            std::cout << "# ERR: " << e.what(); // Print error message
            std::cout << " (MySQL error code: " << e.getErrorCode(); // Print MySQL error code
            std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl; // Print SQL state
            }
    }

//Function that prints a given table
    void Database::printTable(const std::string table){
        try {
            std::string selectQuery = "SELECT * FROM `" + table + "`"; //Print the contents of the table
        result = statement->executeQuery(selectQuery);
        std::cout << "Contents of " << table << ":" << std::endl;
        while (result->next()) {
            std::cout << "ID: " << result->getInt("id") << ", Label: " << result->getString("label") << std::endl;
        }
        std::cout << "Table printed" << std::endl;
        delete result; // Release result set
        }
        catch (sql::SQLException &e) { // Catch MySQL exceptions
            std::cout << "# ERR: " << e.what(); // Print error message
            std::cout << " (MySQL error code: " << e.getErrorCode(); // Print MySQL error code
            std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl; // Print SQL state
            }

    }

    void Database::deletePointers(){
        delete statement; // Delete the statement object
        delete connection; // Delete the connection object
        std::cout << "Pointers er slettet" << std::endl;
    }
