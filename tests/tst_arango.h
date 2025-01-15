#pragma once

#include <gtest/gtest.h>
//#include <gmock/gmock-matchers.h>

#include "arango-cpp/arangoquery.h"
#include "arango-cpp/arangoconnect.h"
#include "arango-cpp/arangocollection.h"

namespace arangocpp {
  ArangoDBConnection connectFromSettings( const std::string& jsonstr, bool rootdata );
}

using namespace testing;
using namespace arangocpp;

TEST(JSONARANGO, TestGenerateRETURN)
{
    EXPECT_EQ( ArangoDBQuery::generateRETURN( true, {}, "u"), "\nRETURN DISTINCT u ");
    EXPECT_EQ( ArangoDBQuery::generateRETURN( false, {}, "u"), "\nRETURN u ");

    QueryFields qfields = {
        { "foo.baz[3].boo", "one" },
        { "bar", "two" },  };

    EXPECT_EQ( ArangoDBQuery::generateRETURN( true, qfields, "u"),
               "\nRETURN DISTINCT  {  \"two\" : u.bar,  \"one\" : u.foo.baz[3].boo } ");
    EXPECT_EQ( ArangoDBQuery::generateRETURN( false, qfields, "u"),
               "\nRETURN  {  \"two\" : u.bar,  \"one\" : u.foo.baz[3].boo } ");
}

TEST(JSONARANGO, TestGenerateFILTER)
{
    FieldValueMap fieldValue = {
        { "foo.baz[3].boo", "1"},
        { "bar", "\"string\""},  };

    // template
    EXPECT_EQ( ArangoDBQuery::generateFILTER( {}, true, "u"), "");
    EXPECT_EQ( ArangoDBQuery::generateFILTER( {{ "bar", "1"}}, true, "u"), " \"bar\" : 1 ");
    EXPECT_EQ( ArangoDBQuery::generateFILTER( fieldValue, true, "u"), " \"bar\" : \"string\" ,  \"foo.baz[3].boo\" : 1 ");
    // AQL
    EXPECT_EQ( ArangoDBQuery::generateFILTER( {}, false, "u"), "");
    EXPECT_EQ( ArangoDBQuery::generateFILTER( {{ "bar", "1"}}, false, "u"), "\nFILTER u.bar == 1 ");
    EXPECT_EQ( ArangoDBQuery::generateFILTER( fieldValue, false, "u"), "\nFILTER u.bar == \"string\" && u.foo.baz[3].boo == 1 ");
}

TEST(JSONARANGO, ArangoConnection )
{
   ArangoDBConnection mydb_data( "http://localhost:8529", "myuser", "mypasswd", "mydatabase");
   EXPECT_EQ( mydb_data.fullHost(), "http://localhost:8529/_db/mydatabase");
   EXPECT_EQ( mydb_data.fullURL( "/_api/version" ), "http://localhost:8529/_db/mydatabase/_api/version");
}

TEST(JSONARANGO, connectFromSettings )
{
   std::string settings = R"({
       "arangodb" :   {
            "UseArangoDBInstance" :   "ArangoDBLocal",
            "UseVelocypackPut" :   true,
            "UseVelocypackGet" :   true,
            "ArangoDBLocal" :   {
                 "DBName" :   "mydatabase",
                 "DBCreate" :   true,
                 "DB_URL" :   "http://localhost:8529",
                 "DBUser" :   "myuser",
                 "DBUserPassword" :   "mypasswd",
                 "DBAccess" :   "rw",
                 "DBRootName" :   "_system",
                 "DBRootUser" :   "root",
                 "DBRootPassword" :   "rootpswd"
            },
            "ArangoDBRemote" :   {
                 "DBName" :   "hub_test",
                 "DBCreate" :   false,
                 "DB_URL" :   "https://db.thermohub.net",
                 "DBUser" :   "__put_here_the_user_name__",
                 "DBUserPassword" :   "__put_here_the_remote_password__",
                 "DBAccess" :   "ro",
                 "DBRootName" :   "_system",
                 "DBRootUser" :   "",
                 "DBRootPassword" :   ""
            }
       }
  })";

  auto conect =  connectFromSettings( settings, false );
  EXPECT_EQ( conect.serverUrl, "http://localhost:8529");
  EXPECT_EQ( conect.databaseName, "mydatabase");
  EXPECT_EQ( conect.user.name, "myuser");
  EXPECT_EQ( conect.user.password, "mypasswd");
  EXPECT_EQ( conect.user.access, "rw");

  auto root_conect =  connectFromSettings( settings, true );
  EXPECT_EQ( root_conect.serverUrl, "http://localhost:8529");
  EXPECT_EQ( root_conect.databaseName, "_system");
  EXPECT_EQ( root_conect.user.name, "root");
  EXPECT_EQ( root_conect.user.password, "rootpswd");
  EXPECT_EQ( root_conect.user.access, "rw");
}

TEST(JSONARANGO, ArangoSanitizingKey )
{
   std::string illegal_key = "   test ü цемент № 7 -:.@()+,=;$!*'";
   auto sanitized = ArangoDBAPIBase::sanitization(illegal_key);
   EXPECT_EQ( sanitized, "test_7_-:.@()+,=;$!*'");
}


#ifdef TestLocalServer

TEST(JSONARANGO, ArangoConnectionErrorLocal )
{
   ArangoDBConnection err_data("http://localhost:8529", "root", "", "test_db_api");
   arangocpp::ArangoDBCollectionAPI connect{err_data};
   EXPECT_TRUE(connect.testConnection());
   EXPECT_EQ(connect.lastError(), "");
   EXPECT_EQ(connect.lastErrorNum(), 0);

   connect.resetDBConnection(ArangoDBConnection{"http://localhost:8529", "root", "", "error_db_name"});
   EXPECT_FALSE(connect.testConnection());
   EXPECT_EQ(connect.lastError(), "database not found");
   EXPECT_EQ(connect.lastErrorNum(), 1228);

   connect.resetDBConnection(ArangoDBConnection{"http://localhost:8529", "error_root", "", "test_db_api"});
   EXPECT_FALSE(connect.testConnection());
   EXPECT_EQ(connect.lastError(), "not authorized to execute this request");
   EXPECT_EQ(connect.lastErrorNum(), 11);

   connect.resetDBConnection(ArangoDBConnection{"http://localhost:8529", "root", "error_pswd", "test_db_api"});
   EXPECT_FALSE(connect.testConnection());
   EXPECT_EQ(connect.lastError(), "not authorized to execute this request");
   EXPECT_EQ(connect.lastErrorNum(), 11);

   connect.resetDBConnection(ArangoDBConnection{"http://errorhost:8529", "root", "", "test_db_api"});
   EXPECT_FALSE(connect.testConnection());
   EXPECT_EQ(connect.lastError(), "connections error to http://errorhost:8529/_db/test_db_api/_api/version");
   EXPECT_EQ(connect.lastErrorNum(), 0);

}
#endif

#ifdef TestRemoteServer

TEST(JSONARANGO, ArangoConnectionErrorRemote )
{
   ArangoDBConnection err_data("https://db.thermohub.net", "test_api_user", "TestApiUser@Remote-ThermoHub-Server", "test_db_api");
   arangocpp::ArangoDBCollectionAPI connect{err_data};
   EXPECT_TRUE(connect.testConnection());
   EXPECT_EQ(connect.lastError(), "");
   EXPECT_EQ(connect.lastErrorNum(), 0);

   connect.resetDBConnection(ArangoDBConnection{"https://db.thermohub.net", "test_api_user", "TestApiUser@Remote-ThermoHub-Server", "error_db_name"});
   EXPECT_FALSE(connect.testConnection());
   EXPECT_EQ(connect.lastError(), "forbidden");
   EXPECT_EQ(connect.lastErrorNum(), 11);

   connect.resetDBConnection(ArangoDBConnection{"https://db.thermohub.net", "error_user", "TestApiUser@Remote-ThermoHub-Server", "test_db_api"});
   EXPECT_FALSE(connect.testConnection());
   EXPECT_EQ(connect.lastError(), "not authorized to execute this request");
   EXPECT_EQ(connect.lastErrorNum(), 11);

   connect.resetDBConnection(ArangoDBConnection{"https://db.thermohub.net", "test_api_user", "error_pswd", "test_db_api"});
   EXPECT_FALSE(connect.testConnection());
   EXPECT_EQ(connect.lastError(), "not authorized to execute this request");
   EXPECT_EQ(connect.lastErrorNum(), 11);

   connect.resetDBConnection(ArangoDBConnection{"https://db.error.thermohub.net", "test_api_user", "TestApiUser@Remote-ThermoHub-Server", "test_db_api"});
   EXPECT_FALSE(connect.testConnection());
   EXPECT_EQ(connect.lastError(), "connections error to https://db.error.thermohub.net/_db/test_db_api/_api/version");
   EXPECT_EQ(connect.lastErrorNum(), 0);
}
#endif
