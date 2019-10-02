/// Test example for ArangoDBCollectionAPI the API for manipulating collections and documents into.

#include <iostream>
#include "jsonarango/arangocollection.h"
#include "jsonarango/arangoexception.h"
#include <velocypack/Collection.h>

void printData( const std::string&  title, const std::vector<std::string>& values )
{
    std::cout <<  title <<  std::endl;
    for( const auto& jsondata: values)
        std::cout <<  jsondata <<  std::endl;
    std::cout <<  std::endl;
}


/// Test different query types
int main(int, char* [])
{
    std::string settingsFileName = "examples-cfg.json";

    // Test collection name
    std::string collectionName = "test";

    // Select from number of documents
    int numOfDocuments =  10;

    // Record keys
    std::vector<std::string> recKeys;
    std::vector<std::string> recjsonValues;

    try{

        // Get Arangodb connection data( load settings from "examples-cfg.json" config file )
        arangocpp::ArangoDBConnect data = arangocpp::connectFromConfig( "examples-cfg.json" );
        // Create database connection
        arangocpp::ArangoDBCollectionAPI connect{data};

        // If document collection collectionName not exist it would be created
        connect.createCollection(collectionName, "vertex");

        // Insert documents to database
        for( int ii=0; ii<numOfDocuments; ii++ )
        {
            ::arangodb::velocypack::Builder builder;
            builder.openObject();
            builder.add("name" , ::arangodb::velocypack::Value(  ii%2 ? "a" : "b" ) );
            builder.add("index" , ::arangodb::velocypack::Value(  ii ) );
            builder.add("task" , ::arangodb::velocypack::Value("exampleQuery"));
            builder.add("properties", ::arangodb::velocypack::Value(::arangodb::velocypack::ValueType::Object));
            builder.add("value" , ::arangodb::velocypack::Value(  10.01*ii ) );
            builder.close();
            builder.close();

            auto rkey = connect.createRecord( collectionName, builder.toJson() );
            recKeys.push_back(rkey);
        }

        std::string aql = "FOR u IN " + collectionName +
                          "\nFILTER u.name == 'a' \n"
                          "RETURN u";
        arangocpp::ArangoDBQuery    aqlquery( aql, arangocpp::ArangoDBQuery::AQL );


        // Define call back function
        arangocpp::SetReadedFunction setfnc = [&recjsonValues]( const std::string& jsondata )
        {
            recjsonValues.push_back(jsondata);
        };

        // Load by keys list
        connect.lookupByKeys( collectionName, recKeys, setfnc );
        printData( "Load by keys list", recjsonValues );

        // Load by query
        recjsonValues.clear();
        connect.selectQuery( collectionName, aqlquery, setfnc );
        printData( "Load by query", recjsonValues );

        // Define call back function
        arangocpp::SetReadedFunctionKey setfnckey = [&recjsonValues]( const std::string& jsondata, const std::string&  )
        {
            recjsonValues.push_back(jsondata);
        };

        // Fetches all documents  from a collection
        recjsonValues.clear();
        connect.selectAll( collectionName, {}, setfnckey );
        printData( "Fetches all documents keys", recjsonValues );

        // Fetches fields  (_id and "index") from all documents from a collection
        recjsonValues.clear();
        connect.selectAll( collectionName, { { "_id", "_id" }, { "index", "index" } }, setfnckey );
        printData( "Fetches all documents keys", recjsonValues );

        // Select field values
        recjsonValues.clear();
        connect.collectQuery( collectionName,"name", recjsonValues );
        printData( "Select field values 'name'", recjsonValues );
        recjsonValues.clear();
        connect.collectQuery( collectionName,"properties.value", recjsonValues );
        printData( "Select field values 'properties.value'", recjsonValues );

        // delete all
        connect.removeByKeys( collectionName, recKeys );
        std::cout <<  "Finish test " <<  std::endl;

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
