/// Test example for TDBJsonDocument CRUD

#include <iostream>
#include "jsonio/dbconnect.h"
#include "jsonio/dbjsondoc.h"
#include "jsonio/io_settings.h"

int main(int, char* [])
{
    jsonio::JsonioSettings::settingsFileName = "examples-cfg.json";
    std::string collectionName = "test";

    try{

        // Create database connection ( load settings from "examples-cfg.json" config file )
        jsonio::TDataBase  database;

        // Create document for collection collectionName
        // If collection not exist it would be created
        std::shared_ptr<jsonio::TDBJsonDocument>  document(
                    jsonio::TDBJsonDocument::newJsonDocument( &database, collectionName ));

        std::cout << "Document : " << "test/eCRUD" <<
                     " exist " << document->Find("test/eCRUD") <<  std::endl;

        // Set data to document
        document->SetJson("{ \"task\" : \"exampleCRUD\", "
                          "  \"properties\" : { \"level\": \"insert record\" } "
                          "}");

        // Insert document to database
        auto rkey = document->Create("test/eCRUD");
        //auto rkey = document->Create("test/FB'FKDB'_D_M");
        //auto rkey = document->Create("test/fgndgn_!_$@_@$_ldkfbnso");

        // Read document from database
        document->Read( rkey );

        // Extract data from document
        std::cout << "Inserted document : " << rkey <<
                     "\n" << document->GetJson(true) <<  std::endl;

        //
        std::cout << "Document : " << rkey <<
                     " exist " << document->Find(rkey) <<  std::endl;


        // Update document
        document->setValue("properties.level", "update record" );
        document->setValue("properties.ivalue", 1 );
        document->setValue("properties.dvalue", 1.5 );
        document->setValue("comment", "test update" );

        // Save changed document to database
        document->Update( rkey );

        // Read record after update
        document->Read( rkey );

        // Extract data from document
        std::cout << "Updated document :\n" << document->GetJson() <<  std::endl;

        // Delete record
        document->Delete( rkey );

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
