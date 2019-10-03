#include <iostream>
#include "jsonarango/arangocollection.h"
#include "jsonarango/arangoexception.h"
#include <velocypack/Collection.h>

int main(int, char* [])
{
    std::string databaseURL = "http://localhost:8529";
    std::string databaseName = "mydatabase";
    std::string collectionName = "mydocuments";

    try{
        // Set ArangoDB root connection data (URL, user name, password, database name)
        arangocpp::ArangoDBConnection root_data( databaseURL, "root", "", "_system");
        // Create a database root connection
        arangocpp::ArangoDBRootClient root_connect{root_data};

        // Define ArangoDB users
        arangocpp::ArangoDBUser dbuser( "myuser", "mypasswd", "rw");
        // Create a new database "mydatabase" (or get to existing one)
        root_connect.createDatabase(databaseName, {dbuser} );

        // Set ArangoDB connection data (URL, user name, password, database name)
        arangocpp::ArangoDBConnection mydb_data( databaseURL, dbuser.name, dbuser.password, databaseName);
        // Create a database connection
        arangocpp::ArangoDBCollectionAPI mydb_connect{mydb_data};


        // Create a new collection "mydocuments" (or get to existing one)
        mydb_connect.createCollection( collectionName, "vertex");

        // Set data to document
        std::string documentData = "{ \"_key\" : \"CRUD\", "
                                   "  \"baz\": [1, 2, 3], "
                                   "  \"foo\" : { \"baz\": \"boo\" } "
                                   "}";


        // Insert a new document to database collection and retrieve its handle
        auto documentHandle = mydb_connect.createDocument( collectionName, documentData );


        // Read a document from database collection
        std::string readDocumentData;
        mydb_connect.readDocument( collectionName, documentHandle,  readDocumentData);
        std::cout << "Created document : " << documentHandle << "\n" << readDocumentData <<  std::endl;

        // Modify the document in readDocumentData
        std::string patch = "{\"add\":\"test update\",\"foo\":{\"add\":1.5 } }";

        auto record = ::arangodb::velocypack::Parser::fromJson(readDocumentData);
        ::arangodb::velocypack::Slice srecord(record->start());

        auto addpatch = ::arangodb::velocypack::Parser::fromJson(patch);
        ::arangodb::velocypack::Slice saddrecord(addpatch->start());

        auto bilder = ::arangodb::velocypack::Collection::merge(srecord, saddrecord, true, false );
        readDocumentData = bilder.toJson();
        std::cout << "Modified document : \n" << readDocumentData <<  std::endl;


        // Save the modified document to database collection
        mydb_connect.updateDocument( collectionName, documentHandle, readDocumentData );

        // Read a document from database collection
        mydb_connect.readDocument( collectionName, documentHandle,  readDocumentData);
        std::cout << "Updated document :\n" << readDocumentData <<  std::endl;

        // Delete the document from database collection
        mydb_connect.deleteDocument( collectionName, documentHandle );

    }
    catch(arangocpp::arango_exception& e)
    {
        std::cout << e.header() << e.what() <<  std::endl;
    }
    catch(std::exception& e)
    {
        std::cout <<  " std::exception" << e.what() <<  std::endl;
    }
    catch(...)
    {
        std::cout <<  " unknown exception" <<  std::endl;
    }

    return 0;
}


