/// Test example for TDBJsonDocument selection

#include <iostream>
#include "jsonio/dbconnect.h"
#include "jsonio/dbjsondoc.h"
#include "jsonio/io_settings.h"

void printData( const std::string&  title,
                const std::vector<std::string>& queryFields,
                const jsonio::ValuesTable& values )
{
    std::cout <<  title <<  std::endl;
    for( const auto& flddata: queryFields)
       std::cout <<  flddata << "\t";

    std::cout <<  std::endl;
    for( const auto& row: values)
    {
        for( const auto& jsondata: row)
           std::cout <<  jsondata << "\t";
        std::cout << std::endl;
    }
}


/// Test different query types
int main(int, char* [])
{
    jsonio::JsonioSettings::settingsFileName = "examples-cfg.json";

    // Test collection name
    std::string collectionName = "test";

    // Select from number of documents
    int numOfDocuments =  10;

    // Record keys
    std::vector<std::string> recKeys;
    jsonio::ValuesTable recValues;

    try{

        // Create database connection ( load settings from "examples-cfg.json" config file )
        jsonio::TDataBase  database;

        // Create document for collection collectionName
        // If collection not exist it would be created
        std::shared_ptr<jsonio::TDBJsonDocument>  document(
                    jsonio::TDBJsonDocument::newJsonDocument( &database, collectionName ));

        // Set data to document
        document->SetJson("{ \"name\" : \"a\",  "
                          "  \"index\" : 1,  "
                          "  \"task\" : \"exampleQuery\",  "
                          "  \"properties\" : { \"value\": 2.5 } "
                          "}");

        // Insert documents to database
        for( int ii=0; ii<numOfDocuments; ii++ )
        {
            if( ii%2 )
                document->setValue("name", "a" );
            else
                document->setValue("name", "b" );
            document->setValue("index", ii );
            document->setValue("properties.value", 10.01*ii );
            auto key = document->Create();
            recKeys.push_back(key);
        }

        std::string aql = "FOR u IN test \n"
                          "FILTER u.name == 'a' \n"
                          "RETURN u";
        jsonio::DBQueryData    aqlquery( aql, jsonio::DBQueryData::qAQL );
        std::vector<std::string> queryFields =  { "_id", "name", "index", "properties.value" };

        // Load by keys list
        recValues = document->downloadDocuments( recKeys, queryFields );
        printData( "Load by keys list", queryFields, recValues );

        // Load by query
        recValues = document->downloadDocuments( aqlquery, queryFields );
        printData( "Load by query", queryFields, recValues );

        // delete all
        document->removeByKeys(  recKeys  );
        std::cout <<  "Finish test " <<  std::endl;

    }
    catch(jsonio::jsonio_exception& e)
    {
        std::cout << "TDBJsonDocument API " << e.title() << " " << e.what() << " " <<e.field()<<  std::endl;
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
