#include "Database.h"
#include <iostream>

// -> bruges for at indikere at vi bruger en memberfunktion i vores pointerobjekter

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

    void Database::useDatabase(){
        try {
            //statement->execute("DROP DATABASE IF EXISTS test_db"); // Drop 'test_db' database if it exists
            //statement->execute("CREATE DATABASE test_db"); // Create 'test_db' database
            statement->execute("USE test_db"); // Set 'test_db' as the current database
            std::cout << "Database i brug" << std::endl;
        }
            catch (sql::SQLException &e) { // Catch MySQL exceptions
                std::cout << "# ERR: " << e.what(); // Print error message
                std::cout << " (MySQL error code: " << e.getErrorCode(); // Print MySQL error code
                std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl; // Print SQL state
            }
    }

    void Database::createTable(){
        try {   
            //statement = connection->createStatement();
            statement->execute("DROP TABLE IF EXISTS neeks"); // Drop 'test' table if it exists
            statement->execute("CREATE TABLE neeks(id INT, label CHAR(25))"); // Create 'neeks' table with id (INT) and label (CHAR(25))
            std::cout << "Tabel oprettet" << std::endl;
        }
        catch (sql::SQLException &e) { // Catch MySQL exceptions
            std::cout << "# ERR: " << e.what(); // Print error message
            std::cout << " (MySQL error code: " << e.getErrorCode(); // Print MySQL error code
            std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl; // Print SQL state
            }
    }

    void Database::insertValues(){
        try {   
            statement->execute("INSERT INTO neeks(id, label) VALUES (1, 'Lille Neek'), (2, 'Medium Neek'), (3, 'Stor neek')"); // Insert neeks into 'greb' table
            std::cout << "Indsat værdier i tabel" << std::endl;
        }
        catch (sql::SQLException &e) { // Catch MySQL exceptions
            std::cout << "# ERR: " << e.what(); // Print error message
            std::cout << " (MySQL error code: " << e.getErrorCode(); // Print MySQL error code
            std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl; // Print SQL state
            }
    }

    void Database::printTable(){
        try {
            result = statement->executeQuery("SELECT * FROM neeks");
            std::cout << "Contents of the neeks table:" << std::endl;
            while (result->next()) {
            std::cout << "ID: " << result->getInt("id") << ", Label: " << result->getString("label") << std::endl;
             }
            // Print the contents of the table greb
            result = statement->executeQuery("SELECT * FROM greb");
            std::cout << "Contents of the greb table:" << std::endl;
            while (result->next()) {
            std::cout << "ID: " << result->getInt("id") << ", Label: " << result->getString("label") << std::endl;
            }
            std::cout << "Tabel Printet" << std::endl;

        }
        catch (sql::SQLException &e) { // Catch MySQL exceptions
            std::cout << "# ERR: " << e.what(); // Print error message
            std::cout << " (MySQL error code: " << e.getErrorCode(); // Print MySQL error code
            std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl; // Print SQL state
            }

    }

    void Database::deletePointers(){
        delete result; // Delete the result set object
        delete statement; // Delete the statement object
        delete connection; // Delete the connection object
        std::cout << "Pointers er slettet" << std::endl;
    }

