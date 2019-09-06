/// Test example for TDBJsonDocument selection

#include <iostream>
#include "jsonio/dbconnect.h"
#include "jsonio/dbjsondoc.h"
#include "jsonio/io_settings.h"


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

        // backup execute function example
        jsonio::SetReadedFunction setfnc = []( const std::string& jsondata)
        {
            std::cout <<  jsondata <<  std::endl;
        };

        std::cout <<  "jsonio::DBQueryData::qAll result " <<  std::endl;
        jsonio::DBQueryData    allquery( jsonio::DBQueryData::qAll );
        document->runQuery( allquery, setfnc );

        std::cout <<  "jsonio::DBQueryData::qTemplate result " <<  std::endl;
        jsonio::DBQueryData    templatequery( "{ \"name\" : \"a\" }", jsonio::DBQueryData::qTemplate );
        document->runQuery( templatequery, setfnc );


        std::cout <<  "jsonio::DBQueryData::qAQL result " <<  std::endl;
        std::string aql = "FOR u IN test \n"
                          "FILTER u.properties.value > 50 \n"
                          "RETURN { \"_id\": u._id, \"name\":u.name, \"index\":u.index }";
        jsonio::DBQueryData    aqlquery( aql, jsonio::DBQueryData::qAQL );
        document->runQuery( aqlquery, setfnc );


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
