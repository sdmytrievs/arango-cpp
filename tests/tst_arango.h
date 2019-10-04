#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "jsonarango/arangoquery.h"
#include "jsonarango/arangoconnect.h"

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
        { "one", "foo.baz[3].boo"},
        { "two", "bar"},  };

    EXPECT_EQ( ArangoDBQuery::generateRETURN( true, qfields, "u"),
               "\nRETURN DISTINCT  {  \"one\" : u.foo.baz[3].boo,  \"two\" : u.bar } ");
    EXPECT_EQ( ArangoDBQuery::generateRETURN( false, qfields, "u"),
               "\nRETURN  {  \"one\" : u.foo.baz[3].boo,  \"two\" : u.bar } ");
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
                 "DBUser" :   "adminrem",
                 "DBUserPassword" :   "Administrator@Remote-ThermoHub-Server",
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
