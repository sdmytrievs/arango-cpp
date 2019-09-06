/// Test example for TDBJsonDocument CRUD

#include <iostream>
#include "jsonio/dbconnect.h"
#include "jsonio/dbjsondoc.h"
#include "jsonio/io_settings.h"

int main(int, char* [])
{
    jsonio::JsonioSettings::settingsFileName = "examples-cfg.json";

    try{

        jsonio::JsonioSettings& settings = jsonio::ioSettings();
        jsonio::GroupSettings commongroup = settings.group("common");

        std::cout << "SchemasDirectory : " << commongroup.value( "SchemasDirectory", std::string("") ) <<  std::endl;
        std::cout << "UserHomeDirectoryPath : " << commongroup.value( "UserHomeDirectoryPath", std::string(".") ) <<  std::endl;

        // Extract data from "jsonio" settings

        std::cout << "UseVelocypackPut : " << settings.useVelocypackPut() <<  std::endl;
        std::cout << "ResourcesDirectory : " << settings.resourcesDir() <<  std::endl;
        std::cout << "ArangoDBLocal : " << settings.value( "jsonio.ArangoDBLocal", std::string("") ) <<  std::endl;

        // Add test block
        jsonio::GroupSettings testgroup = settings.group("test");

        testgroup.setValue( "string", std::string("Hello!") );
        testgroup.setValue( "bool", true );
        testgroup.setValue( "number", 10 );
        testgroup.setValue( "float", 2.5 );
        testgroup.setValue( "const_char", "const char *" );

        std::cout << "Test block" <<  std::endl;
        std::cout << "string : " << testgroup.value( "string", std::string("Hi!") ) <<  std::endl;
        std::cout << "bool : " << testgroup.value( "bool", false ) <<  std::endl;
        std::cout << "number : " << testgroup.value( "number", 100 ) <<  std::endl;
        std::cout << "float : " << testgroup.value( "float", 22.9 ) <<  std::endl;
        std::cout << "const_char : " << testgroup.value( "const_char", "!!!" ) <<  std::endl;

       // Test complex path
       settings.setValue( "test2.struct.int", 5 );
       settings.setValue( "test2.string", "any string" );

       std::cout << "Test complex path" <<  std::endl;
       std::cout << "test2.string : " << settings.value( "test2.string", std::string("???") ) <<  std::endl;
       std::cout << "test2.struct.int : " << settings.value( "test2.struct.int", 50 ) <<  std::endl;
       std::cout << "test2.struct : " << settings.value( "test2.struct", std::string("") ) <<  std::endl;

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
