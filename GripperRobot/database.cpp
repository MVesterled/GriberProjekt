#include "database.h"
#include <iostream>

// -> bruges for at indikere at vi bruger en memberfunktion i vores pointerobjekter

//Constructor that makes connection to the database
Database::Database() {
    std::cout << "Pr�ver at lave forbindelse..." << std::endl;
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
    void Database::createTableGreb(std::string tableName){
        try {
            //std::string dropQuery = "DROP TABLE IF EXISTS `" + tableName + "`"; // Drop 'tableName' table if it exists
            //statement->execute(dropQuery);

            std::string createQuery = "CREATE TABLE `" + tableName + "` (greb_id INT AUTO_INCREMENT PRIMARY KEY, label CHAR(250))"; // Create 'tableName' table with id (INT) and label (CHAR(25))
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
    void Database::insertValuesGreb(std::string table, std::string label){
        try {
            std::string insertQuery = "INSERT INTO `" + table + "` (label) VALUES ('" + label + "')"; // Insert values into table
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
    void Database::printTableGreb(const std::string table){
        try {
            std::string selectQuery = "SELECT * FROM `" + table + "`"; //Print the contents of the table
        result = statement->executeQuery(selectQuery);
        std::cout << "Contents of " << table << ":" << std::endl;
        while (result->next()) {
            std::cout << "ID: " << result->getInt("greb_id") << ", Label: " << result->getString("label") << std::endl;
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

    void Database::createTablePose(std::string tableName){
        std::string createQuery = "CREATE TABLE `" + tableName + "` ("
                                                                 "pose_id INT AUTO_INCREMENT, "
                                                                 "j1 DOUBLE, "
                                                                 "j2 DOUBLE, "
                                                                 "j3 DOUBLE, "
                                                                 "j4 DOUBLE, "
                                                                 "j5 DOUBLE, "
                                                                 "j6 DOUBLE, "
                                                                 "PRIMARY KEY (pose_id))";
                                                                 /*"FOREIGN KEY (pose_id) REFERENCES Greb(greb_id))"; // Replace 'other_table' with the actual table name containing 'gerb_id'*/
        statement->execute(createQuery);
    }

    void Database::insertValuesPose(std::string table, double j1, double j2, double j3, double j4, double j5, double j6){
        try {
            std::string insertQuery = "INSERT INTO `" + table + "` (j1, j2, j3, j4, j5, j6) VALUES ("
                                      + std::to_string(j1) + ", "
                                      + std::to_string(j2) + ", "
                                      + std::to_string(j3) + ", "
                                      + std::to_string(j4) + ", "
                                      + std::to_string(j5) + ", "
                                      + std::to_string(j6) + ")"; // Insert values into Posetable
            statement->execute(insertQuery);
            std::cout << "Values inserted into table" << std::endl;
        }
        catch (sql::SQLException &e) { // Catch MySQL exceptions
            std::cout << "# ERR: " << e.what(); // Print error message
            std::cout << " (MySQL error code: " << e.getErrorCode(); // Print MySQL error code
            std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl; // Print SQL state
        }
    }

    void Database::printTablePose(const std::string table){
        try {
            std::string selectQuery = "SELECT * FROM `" + table + "`"; // Print the contents of the table
            result = statement->executeQuery(selectQuery);
            std::cout << "Contents of " << table << ":" << std::endl;
            while (result->next()) {
                std::cout << "pose_id: " << result->getInt("pose_id") << ", "
                          << "j1: " << result->getDouble("j1") << ", "
                          << "j2: " << result->getDouble("j2") << ", "
                          << "j3: " << result->getDouble("j3") << ", "
                          << "j4: " << result->getDouble("j4") << ", "
                          << "j5: " << result->getDouble("j5") << ", "
                          << "j6: " << result->getDouble("j6") << std::endl;
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
