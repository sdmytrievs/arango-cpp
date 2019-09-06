/// Test example for TDBJsonDocument selection

#include <iostream>
#include "jsonio/dbconnect.h"
#include "jsonio/dbjsondoc.h"
#include "jsonio/io_settings.h"

void printData( const std::string&  title, const std::vector<std::string>& values )
{
    std::cout <<  title <<  std::endl;
    for( const auto& jsondata: values)
        std::cout <<  jsondata <<  std::endl;
}

int main(int, char* [])
{
    jsonio::JsonioSettings::settingsFileName = "examples-cfg.json";

    // Test collection name
    std::string collectionName = "test";

    // Non standard record keys
    std::vector<std::string> keyFldsInf = { "name",  "index" };

    // Select from number of documents
    int numOfDocuments =  10;

    // Record keys
    std::vector<std::string> recKeys;
    std::vector<std::string> recjsonValues;

    try{

        // Create database connection ( load settings from "examples-cfg.json" config file )
        jsonio::TDataBase  database;

        // Create document for collection collectionName
        // If collection not exist it would be created
        std::shared_ptr<jsonio::TDBJsonDocument>  document(
                    jsonio::TDBJsonDocument::newJsonDocument( &database, collectionName, keyFldsInf ));

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

            // create/update by key
            //std::string key = "test/a_"+std::to_string(ii);
            //document->Update( key );

            // create by key
            //std::string key = "test/a_"+std::to_string(ii);
            //document->Create( key );

            // create key from template
            auto key = document->Create();

            recKeys.push_back(key);
        }

        printData( "Keys list", recKeys );

        // Load by keys list
        recjsonValues = document->runByKeys( recKeys );
        printData( "Load by keys list", recjsonValues );

        // Load by query
        jsonio::DBQueryData    allquery( jsonio::DBQueryData::qAll );
        recjsonValues = document->runQuery( allquery );
        printData( "Load by query", recjsonValues );

        // delete all
        document->removeByKeys(  recKeys  );
        std::cout <<  "Finish test " <<  std::endl;

        //??? CRUD
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
