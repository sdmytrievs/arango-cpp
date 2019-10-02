/// Test example for ArangoDBCollectionAPI the API for manipulating collections and documents into.

#include <iostream>
#include "jsonarango/arangocollection.h"
#include "jsonarango/arangoexception.h"


void printData( const std::string&  title, const std::set<std::string>& values )
{
    std::cout <<  title <<  std::endl;
    for( const auto& jsondata: values)
        std::cout <<  jsondata <<  "  ";
    std::cout <<  std::endl;
}

int main(int, char* [])
{
    std::string settingsFileName = "examples-cfg.json";
    std::string collectionName = "createTestCollection";

    try{
        // Get Arangodb connection data( load settings from "examples-cfg.json" config file )
        arangocpp::ArangoDBConnect data = arangocpp::connectFromConfig( "examples-cfg.json" );
        // Create database connection
        arangocpp::ArangoDBCollectionAPI connect{data};

        // Get all collections list
        std::set<std::string> collections = connect.collectionNames( arangocpp::CollectionTypes::All );
        printData( "Start Collections list : ", collections );

        // If document collection collectionName not exist it would be created
        connect.createCollection(collectionName, "vertex");
        // Test exist collection
        std::cout << "Collection : " << collectionName << " exist " << connect.existCollection(collectionName) <<  std::endl;

        // Get all collections list
        collections = connect.collectionNames( arangocpp::CollectionTypes::All );
        printData( "After create collection list : ", collections );

        // Delete collectionName not
        connect.dropCollection(collectionName);
        // Test exist collection
        std::cout << "Collection : " << collectionName << " exist " << connect.existCollection(collectionName) <<  std::endl;

        // Get all collections list
        collections = connect.collectionNames( arangocpp::CollectionTypes::All );
        printData( "After drop collection list : ", collections );

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


