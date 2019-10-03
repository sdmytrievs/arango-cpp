/// Test example for ArangoDBUsersAPI API to Create/Delete ArangoDB users and databases.

#include <iostream>
#include "jsonarango/arangodbusers.h"
#include "jsonarango/arangoexception.h"


void printData( const std::string&  title, const std::set<std::string>& values )
{
    std::cout <<  title <<  std::endl;
    for( const auto& data: values)
        std::cout <<  data <<  "  ";
    std::cout <<  std::endl;
}

void printData( const std::string&  title, const std::map<std::string,std::string>& values )
{
    std::cout <<  title <<  std::endl;
    for( const auto& mapdata: values)
        std::cout <<  mapdata.first <<  " : " << mapdata.second << ";  ";
    std::cout <<  std::endl;
}


int main(int, char* [])
{
    std::string settingsFileName = "examples-cfg.json";
    std::string databaseName = "test_db_api";

    try{
        // Get Arangodb connection data( load settings from "examples-cfg.json" config file )
        arangocpp::ArangoDBConnection rootdata( "http://localhost:8529", "root", "", "_system");
        // Create database connection
        arangocpp::ArangoDBUsersAPI rootconnect{rootdata};

        // Define ArangoDB users
        arangocpp::ArangoDBUser user1( "user1_test", "passwd1", "rw");
        arangocpp::ArangoDBUser user2( "user2_test", "passwd2", "ro");

        // Get all databases list
        auto databases = rootconnect.databaseNames(  );
        printData( "Start databases list : ", databases );
        // Test exist database
        std::cout << "Database : " << databaseName << " exist " << rootconnect.existDatabase(databaseName) <<  std::endl;

        // If database databaseName not exist it would be created
        rootconnect.createDatabase(databaseName, {user1});
        databases = rootconnect.databaseNames(  );
        printData( "After create databases list : ", databases );
        // Test exist database
        std::cout << "After create Database : " << databaseName << " exist " << rootconnect.existDatabase(databaseName) <<  std::endl;

        // Fetches data about all users.
        std::set<std::string> user_names = rootconnect.userNames();
        printData( "Fetches data about all users : ", user_names );

        // Create user.
        rootconnect.createUser( user2 );

        // Fetches data about all users.
        user_names = rootconnect.userNames();
        printData( "Fetches data about all users after create : ", user_names );

        // Retrieves a map contains the databases names as object keys, and the associated privileges
        // for the database as values of all databases the current user can access.
        auto databasegrand = rootconnect.databaseNames( user2.name );
        printData( "The map contains the databases names : ", databasegrand );

        // Grant or revoke user access to a database ( "rw", "ro" or "none").
        rootconnect.grantUserToDataBase(databaseName, user2.name, "rw" );

        // Retrieves a map contains the databases names as object keys, and the associated privileges
        // for the database as values of all databases the current user can access.
        databasegrand = rootconnect.databaseNames( user2.name );
        printData( "The map contains the databases names (after grand user2) : ", databasegrand );

        // Modify attributes of an existing user.
        user2.password = "passwd3";
        user2.access = "rw";
        rootconnect.updateUser( user2 );

        rootconnect.removeUser( user1.name );
        rootconnect.removeUser( user2.name );

        // Fetches data about all users.
        user_names = rootconnect.userNames();
        printData( "Fetches data about all users after remove: ", user_names );

        // Delete databse
        rootconnect.removeDatabase(databaseName);
        // Test exist database
        std::cout << "After delete Database : " << databaseName << " exist " << rootconnect.existDatabase(databaseName) <<  std::endl;

    }
    catch(arangocpp::arango_exception& e)
    {
        std::cout << "TDBJsonDocument API" << e.header() << e.what() <<  std::endl;
    }
    catch(std::exception& e)
    {
        std::cout <<  "TDBJsonDocument API" << " std::exception" << e.what() <<  std::endl;
    }
    catch(...)
    {
        std::cout <<  "TDBJsonDocument API" << " unknown exception" <<  std::endl;
    }

    return 0;
}

