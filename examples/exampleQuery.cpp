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
        arangocpp::ArangoDBConnection data = arangocpp::connectFromConfig( "examples-cfg.json" );
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

            auto rkey = connect.createDocument( collectionName, builder.toJson() );
            recKeys.push_back(rkey);
        }

        // Define call back function
        arangocpp::FetchingDocumentCallback setfnc = [&recjsonValues]( const std::string& jsondata )
        {
            recjsonValues.push_back(jsondata);
        };

        // Select all records
        recjsonValues.clear();
        arangocpp::ArangoDBQuery    allquery( arangocpp::ArangoDBQuery::All );
        connect.selectQuery( collectionName, allquery, setfnc );
        printData( "Select all records", recjsonValues );

        // Select records by template
        recjsonValues.clear();
        arangocpp::ArangoDBQuery    templatequery( "{ \"name\" : \"a\" }", arangocpp::ArangoDBQuery::Template );
        connect.selectQuery( collectionName, templatequery, setfnc );
        printData( "Select records by template", recjsonValues );

        // Select records by AQL query
        recjsonValues.clear();
        std::string aql = "FOR u IN " + collectionName +
                "\nFILTER u.properties.value > 50 \n"
                "RETURN { \"_id\": u._id, \"name\":u.name, \"index\":u.index }";
        arangocpp::ArangoDBQuery    aqlquery( aql, arangocpp::ArangoDBQuery::AQL );
        connect.selectQuery( collectionName, aqlquery, setfnc );
        printData( "Select records by AQL query", recjsonValues );

        // delete by example
        connect.removeByTemplate( collectionName, "{ \"name\" : \"a\" }" );
        recjsonValues.clear();
        connect.selectQuery( collectionName, allquery, setfnc );
        printData( "All after removing", recjsonValues );

        connect.removeByTemplate( collectionName, "{ \"name\" : \"b\" }" );
        std::cout <<  "Finish test " <<  std::endl;

    }
    catch(arangocpp::arango_exception& e)
    {
        std::cout << "ArangoDBCollectionAPI" << e.header() << e.what() <<  std::endl;
    }
    catch(std::exception& e)
    {
        std::cout <<  "ArangoDBCollectionAPI" << " std::exception" << e.what() <<  std::endl;
    }
    catch(...)
    {
        std::cout <<  "ArangoDBCollectionAPI" << " unknown exception" <<  std::endl;
    }

    return 0;
}
