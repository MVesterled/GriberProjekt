#include <stdlib.h>
#include <iostream>
/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include <TestClass.h>
#include <TestClass2.h>

int main(void) {

    std::cout << "Started" << std::endl;
    TestClass testObject;
    testObject.printTest();
    TestClass2 testObject2;
    testObject2.printTest2();

    try {
        //Creates objects
        sql::Driver *driver; // MySQL driver object
        sql::Connection *con; // MySQL connection object
        sql::Statement *stmt; // MySQL statement object
        sql::ResultSet *res; // MySQL result set object

        /* Create a connection */
        driver = get_driver_instance(); // Get MySQL driver instance
        con = driver->connect("tcp://127.0.0.1:3306", "root", "root"); // Connect to MySQL server (IP Address, user name, password)

        stmt = con->createStatement(); // Create a MySQL statement object
        //stmt->execute("DROP DATABASE IF EXISTS test_db"); // Drop 'test_db' database if it exists
        //stmt->execute("CREATE DATABASE test_db"); // Create 'test_db' database

        stmt->execute("USE test_db"); // Set 'test_db' as the current database
        stmt->execute("DROP TABLE IF EXISTS greb"); // Drop 'test' table if it exists
        stmt->execute("CREATE TABLE greb(id INT, label CHAR(25))"); // Create 'neeks' table with id (INT) and label (CHAR(25))

        stmt->execute("INSERT INTO greb(id, label) VALUES (1, 'Hårdt'), (2, 'Medium'), (3, 'Blødt')"); // Insert data into 'neeks' table
        
        // Print the contents of the table neeks
        res = stmt->executeQuery("SELECT * FROM neeks");
        std::cout << "Contents of the neeks table:" << std::endl;
        while (res->next()) {
            std::cout << "ID: " << res->getInt("id") << ", Label: " << res->getString("label") << std::endl;
        }
        // Print the contents of the table greb
        res = stmt->executeQuery("SELECT * FROM greb");
        std::cout << "Contents of the greb table:" << std::endl;
        while (res->next()) {
            std::cout << "ID: " << res->getInt("id") << ", Label: " << res->getString("label") << std::endl;
        }

        delete res; // Delete the result set object
        delete stmt; // Delete the statement object
        delete con; // Delete the connection object
        /* According to documentation,
        You must free the sql::Statement and sql::Connection objects explicitly using delete
        But do not explicitly free driver, the connector object. Connector/C++ takes care of freeing that. */

    } catch (sql::SQLException &e) { // Catch MySQL exceptions
        std::cout << "# ERR: " << e.what(); // Print error message
        std::cout << " (MySQL error code: " << e.getErrorCode(); // Print MySQL error code
        std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl; // Print SQL state
    }

    std::cout << "Successfully ended" << std::endl; // Print end message
    return EXIT_SUCCESS; // Return success
}
