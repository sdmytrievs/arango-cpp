/// Test example for TDBJsonDocument CRUD

#include <iostream>
#include "jsonio/dbarango.h"
#include "jsonio/dbjsondoc.h"
#include "jsonio/io_settings.h"

int main(int, char* [])
{
    jsonio::JsonioSettings::settingsFileName = "examples-cfg.json";
    std::string collectionName = "test";

    try{

        // Create database connection ( load settings from "examples-cfg.json" config file )
        jsonio::TArangoDBClient  arangoClient;

        // Set data to document
        std::string jsonrec = "{ \"_key\" : \"eCRUD2\",  "
                               "  \"task\" : \"exampleCRUD2\", "
                               "  \"properties\" : { \"level\": \"insert record\" } "
                               "}";

        std::cout << "Document : " << "test/eCRUD" <<
                     " exist " << arangoClient.existsRecord( collectionName,"test/eCRUD") <<  std::endl;

        // Insert document to database
        auto rid = arangoClient.createRecord( collectionName, jsonrec );

        // Read document from database
        arangoClient.readRecord( collectionName, rid, jsonrec );

        // Extract data from document
        std::cout << "Inserted document : " << rid << "\n" << jsonrec <<  std::endl;

        std::cout << "Document : " << "test/eCRUD2" <<
                     " exist " << arangoClient.existsRecord( collectionName,"test/eCRUD2") <<  std::endl;

        // Update document
        // Current json dom  object
        std::shared_ptr<jsonio::JsonDomFree> jsonData(jsonio::JsonDomFree::newObject());
        jsonio::parseJsonToNode( jsonrec, jsonData.get() );
        jsonData->updateFieldPath("properties.level", "update record" );
        jsonData->updateFieldPath("properties.ivalue", std::to_string(1) );
        jsonData->updateFieldPath("properties.dvalue", std::to_string(1.5) );
        jsonData->updateFieldPath("comment", "test update" );

        // Save changed document to database
        arangoClient.updateRecord( collectionName, rid, jsonData->toString(false) );

        // Read record after update
        std::string jsonrecnew;
        arangoClient.readRecord( collectionName, rid, jsonrecnew );

        // Extract data from document
        std::cout << "Updated document :\n" << jsonrecnew <<  std::endl;

        // Delete record
        arangoClient.deleteRecord( collectionName, rid );

    }
    catch(jsonio::jsonio_exception& e)
    {
        std::cout << "TDBJsonDocument API" << e.title() << e.what() << e.field()<<  std::endl;
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

// curl -X GET @- --dump - http://localhost:8529/_api/document/test/eCRUD2 -u root:""
// curl -X HEAD --dump - http://localhost:8529/_api/document/test/eCRUD2 -u root:""
// curl -I --dump - http://localhost:8529/_api/document/test/eCRUD2 -u root:""
//  https://serverfault.com/questions/140149/difference-between-curl-i-and-curl-x-head
