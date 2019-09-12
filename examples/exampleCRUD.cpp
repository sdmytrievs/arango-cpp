/// Test example for ArangoDBCollectionAPI the API for manipulating collections and documents into.

#include <iostream>
#include "arangocollection.h"
#include "arangoexception.h"
#include <velocypack/Collection.h>

int main(int, char* [])
{
    std::string settingsFileName = "examples-cfg.json";
    std::string collectionName = "test";
    std::string documentHandle = "test/eCRUD";

    try{
        // Get Arangodb connection data( load settings from "examples-cfg.json" config file )
        arangocpp::ArangoDBConnect data = arangocpp::connectFromConfig( "examples-cfg.json" );
        // Create database connection
        arangocpp::ArangoDBCollectionAPI connect{data};

        // If document collection collectionName not exist it would be created
        connect.createCollection(collectionName, "vertex");

        std::cout << "Document : " << documentHandle <<
                     " exist " << connect.existsRecord(collectionName, documentHandle) <<  std::endl;

        // Set data to document
        std::string documentData = "{ \"_key\" : \"eCRUD\", "
                                   "  \"task\" : \"exampleCRUD\", "
                                   "  \"properties\" : { \"level\": \"insert record\" } "
                                   "}";

        // Insert document to database
        auto rkey = connect.createRecord( collectionName, documentData );


        // Read document from database
        std::string readDocumentData;
        connect.readRecord( collectionName, rkey,  readDocumentData);

        // Extract data from document
        std::cout << "Inserted document : " << rkey <<
                     "\n" << readDocumentData <<  std::endl;

        // modify document
        std::string addtorecord = "{\"comment\":\"test update\",\"properties\":{\"dvalue\":1.5,\"ivalue\":1,\"level\":\"update record\"} }";

        auto record = ::arangodb::velocypack::Parser::fromJson(readDocumentData);
        ::arangodb::velocypack::Slice srecord(record->start());

        auto addrecord = ::arangodb::velocypack::Parser::fromJson(addtorecord);
        ::arangodb::velocypack::Slice saddrecord(addrecord->start());

        auto bilder = ::arangodb::velocypack::Collection::merge(srecord, saddrecord, true, false );
        documentData = bilder.toJson();
        std::cout << "Modified document : \n" << documentData <<  std::endl;


        // Save changed document to database
        connect.updateRecord( collectionName, rkey, documentData );

        // Read record after update
        connect.readRecord( collectionName, rkey,  readDocumentData);

        // Extract data from document
        std::cout << "Updated document :\n" << readDocumentData <<  std::endl;

        // Delete record
        connect.deleteRecord( collectionName, rkey );

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

// curl -X GET @- --dump - http://localhost:8529/_api/document/test/eCRUD2 -u root:""
// curl -X HEAD --dump - http://localhost:8529/_api/document/test/eCRUD2 -u root:""
// curl -I --dump - http://localhost:8529/_api/document/test/eCRUD2 -u root:""
//  https://serverfault.com/questions/140149/difference-between-curl-i-and-curl-x-head
