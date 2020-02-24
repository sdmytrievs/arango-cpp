#include <iostream>
#include <chrono>

/// Time test example for ArangoDBCollectionAPI the API for manipulating collections and documents into.

#include <iostream>
#include "jsonarango/arangocollection.h"
#include "jsonarango/arangoexception.h"
#include <velocypack/Collection.h>


using time_point_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

// Current number documents into collection
static int documentsInCollection =  1000;


void printData( const std::string&  title, const std::vector<std::string>& values )
{
    std::cout <<  title <<  std::endl;
    for( const auto& jsondata: values)
        std::cout <<  jsondata <<  std::endl;
    std::cout <<  std::endl;
}


void printTime( const std::string&  title, const time_point_t& start, const time_point_t& end )
{
    std::cout <<  title << "\nElapsed time in microseconds: ";
    std::cout <<  std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << " μs" << std::endl;
}

void printTimeSec( const std::string&  title, const time_point_t& start, const time_point_t& end )
{
    std::cout <<  title << "\nElapsed time in seconds: ";
    std::cout <<  std::chrono::duration_cast<std::chrono::seconds>(end-start).count() << "s" << std::endl;
}

/// Test different query types
int different_query_types( arangocpp::ArangoDBCollectionAPI& connect );
int substances_query_types( arangocpp::ArangoDBCollectionAPI& connect );

int main(int argc, char* argv[])
{
    std::string settingsFileName = "examples-cfg.json";

    if( argc > 1)
        documentsInCollection = std::stoi(argv[1]);

    if( argc > 2)
        settingsFileName = argv[2];

    try{

        // Get Arangodb connection data( load settings from "examples-cfg.json" config file )
        arangocpp::ArangoDBConnection data = arangocpp::connectFromConfig( "examples-cfg.json" );
        // Create database connection
        arangocpp::ArangoDBCollectionAPI connect{data};

        //different_query_types( connect );
        substances_query_types( connect );

    }
    catch(...)
    {
        std::cout <<  "  unknown exception" <<  std::endl;
    }
    return 0;
}

// Test different query types
int different_query_types( arangocpp::ArangoDBCollectionAPI& connect )
{
    // Test collection name
    std::string collectionName = "test1";

    // Record keys
    std::vector<std::string> recKeys;
    std::vector<std::string> recjsonValues;
    // Define call back function
    arangocpp::FetchingDocumentCallback setfnc = [&recjsonValues]( const std::string& jsondata )
    {
        recjsonValues.push_back(jsondata);
    };

    auto start = std::chrono::high_resolution_clock::now();

    // If document collection collectionName not exist it would be created
    connect.createCollection(collectionName, "vertex");

    auto end1 = std::chrono::high_resolution_clock::now();
    printTime( "Create collection", start, end1 );

    // Insert documents to database
    for( int ii=0; ii<documentsInCollection; ii++ )
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

    auto end2 = std::chrono::high_resolution_clock::now();
    printTime( "Insert documents to database", end1, end2 );

    // Select all records
    recjsonValues.clear();
    arangocpp::ArangoDBQuery    allquery( arangocpp::ArangoDBQuery::All );
    connect.selectQuery( collectionName, allquery, setfnc );
    //printData( "Select all records", recjsonValues );
    auto end3 = std::chrono::high_resolution_clock::now();
    printTime( "Select all records", end2, end3 );

    // Select records by template
    recjsonValues.clear();
    arangocpp::ArangoDBQuery    templatequery( "{ \"name\" : \"a\" }", arangocpp::ArangoDBQuery::Template );
    connect.selectQuery( collectionName, templatequery, setfnc );
    //printData( "Select records by template", recjsonValues );
    auto end4 = std::chrono::high_resolution_clock::now();
    printTime( "Select records by template", end3, end4 );

    // Select records by AQL query
    recjsonValues.clear();
    std::string aql = "FOR u IN " + collectionName +
            "\nFILTER u.properties.value > 50 \n"
            "RETURN { \"_id\": u._id, \"name\":u.name, \"index\":u.index }";
    arangocpp::ArangoDBQuery    aqlquery( aql, arangocpp::ArangoDBQuery::AQL );
    connect.selectQuery( collectionName, aqlquery, setfnc );
    //printData( "Select records by AQL query", recjsonValues );
    auto end5 = std::chrono::high_resolution_clock::now();
    printTime( "Select records by AQL query", end4, end5 );

    // delete by example
    connect.removeByTemplate( collectionName, "{ \"name\" : \"a\" }" );
    recjsonValues.clear();
    connect.selectQuery( collectionName, allquery, setfnc );
    //printData( "All after removing", recjsonValues );

    connect.removeByTemplate( collectionName, "{ \"name\" : \"b\" }" );
    auto end6 = std::chrono::high_resolution_clock::now();
    printTime( "Delete by example", end5, end6 );

    printTime( "All time", start, end6 );
    return 0;
}


int substances_query_types( arangocpp::ArangoDBCollectionAPI& connect )
{
    // Test collection name
    std::string collectionName = "substances";

    // Record keys
    std::vector<std::string> recKeys;
    std::vector<std::string> recjsonValues;
    // Define call back function
    arangocpp::FetchingDocumentCallback setfnc = [&recjsonValues]( const std::string& jsondata )
    {
        recjsonValues.push_back(jsondata);
    };

    auto start = std::chrono::high_resolution_clock::now();

    // If document collection collectionName not exist it would be created
    connect.createCollection(collectionName, "vertex");

    auto end1 = std::chrono::high_resolution_clock::now();
    printTimeSec( "Create collection", start, end1 );


    // Select all records
    recjsonValues.clear();
    arangocpp::ArangoDBQuery    allquery( arangocpp::ArangoDBQuery::All );
    connect.selectQuery( collectionName, allquery, setfnc );
    //printData( "Select all records", recjsonValues );
    auto end2 = std::chrono::high_resolution_clock::now();
    printTimeSec( "Select all records ( " + std::to_string(recjsonValues.size()) + " )", end1, end2 );

    // Select records by template
    recjsonValues.clear();
    arangocpp::ArangoDBQuery    templatequery( "{ \"_label\" : \"substance\" }", arangocpp::ArangoDBQuery::Template );
    connect.selectQuery( collectionName, templatequery, setfnc );
    //printData( "Select records by template", recjsonValues );
    auto end3 = std::chrono::high_resolution_clock::now();
    printTimeSec( "Select records by template ( " + std::to_string(recjsonValues.size()) + " )", end2, end3 );

    // Select records by AQL query
    recjsonValues.clear();
    std::string aql = "FOR u IN " + collectionName +
            // "\nFILTER u.properties.value > 50 \n"
            "\nRETURN { \"_id\": u._id, \"name\":u.properties.name }";
    arangocpp::ArangoDBQuery    aqlquery( aql, arangocpp::ArangoDBQuery::AQL );
    connect.selectQuery( collectionName, aqlquery, setfnc );
    //printData( "Select records by AQL query", recjsonValues );
    auto end4 = std::chrono::high_resolution_clock::now();
    printTimeSec( "Select records by AQL query ( " + std::to_string(recjsonValues.size()) + " )", end3, end4 );

    printTimeSec( "All time", start, end4 );
    return 0;
}
