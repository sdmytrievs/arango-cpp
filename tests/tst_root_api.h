#pragma once

#include <gtest/gtest.h>
//#include <gmock/gmock-matchers.h>

#include "init_tests.h"
#include "jsonarango/arangodbusers.h"
#include "jsonarango/arangoconnect.h"

using namespace testing;
using namespace arangocpp;


// ConnectionTestF is a test fixture that can be used for all kinds of connection
// tests. You can configure it using the ConnectionTestParams struct.
// Used to test API to Create/Delete ArangoDB users and databases
class RootTestF : public ::testing::TestWithParam< arango_root_t >
{


public:

    const std::string databaseName = "test_root_db";
    arangocpp::ArangoDBUser user1;
    arangocpp::ArangoDBUser user2;

protected:

    RootTestF():user1( "test_user1", "passwd1", "rw"),
        user2( "test_user2", "passwd2", "ro")
    {}
    virtual ~RootTestF() noexcept override {}



    virtual void SetUp() override
    {
        try {
            auto   rootconnect = GetParam();
            rootconnect->createDatabase(databaseName, {user1});
        }
        catch(std::exception const& ex)
        {
            std::cout << "SETUP OF FIXTURE FAILED" << std::endl;
            throw ex;
        }
    }

    virtual void TearDown() override
    {
        // delete collection  "test_crud"
        auto   rootconnect = GetParam();
        rootconnect->removeDatabase(databaseName);
        rootconnect->removeUser( user1.name );
        rootconnect->removeUser( user2.name );
    }

private:

};

INSTANTIATE_TEST_SUITE_P(BasicConnectionTests, RootTestF,
                         ::testing::ValuesIn(rootTestParams));

TEST_P(RootTestF, testDatabaseCreateSelect )
{
    auto   rootconnect = GetParam();
    std::string otherDatabaseName = "test_other_db2";

    EXPECT_TRUE( rootconnect->existDatabase(databaseName) );
    EXPECT_FALSE( rootconnect->existDatabase(otherDatabaseName) );

    // create database
    EXPECT_NO_THROW( rootconnect->createDatabase(otherDatabaseName, {user1, user2} ) );
    ASSERT_TRUE( rootconnect->existDatabase(otherDatabaseName) );
    EXPECT_TRUE( rootconnect->existUser(user1.name) );
    EXPECT_TRUE( rootconnect->existUser(user2.name) );

    // database names
    auto alldatabases = rootconnect->databaseNames();
    EXPECT_NE( alldatabases.find(databaseName), alldatabases.end() );
    EXPECT_NE( alldatabases.find(otherDatabaseName), alldatabases.end() );

    // drop database
    EXPECT_NO_THROW( rootconnect->removeDatabase(otherDatabaseName) );
    EXPECT_FALSE( rootconnect->existDatabase(otherDatabaseName) );

    alldatabases = rootconnect->databaseNames();
    EXPECT_EQ( alldatabases.find(otherDatabaseName), alldatabases.end() );

}

TEST_P(RootTestF, testUserCreateSelect )
{
    auto   rootconnect = GetParam();
    arangocpp::ArangoDBUser otherUser( "test_user3", "passwd3", "rw");

    EXPECT_TRUE( rootconnect->existUser(user1.name) );
    EXPECT_FALSE( rootconnect->existUser(user2.name) );
    EXPECT_FALSE( rootconnect->existUser(otherUser.name) );

    // create user
    EXPECT_NO_THROW( rootconnect->createUser( user2 ) );
    EXPECT_NO_THROW( rootconnect->createUser( otherUser ) );
    ASSERT_TRUE( rootconnect->existUser(otherUser.name) );
    EXPECT_TRUE( rootconnect->existUser(user1.name) );
    EXPECT_TRUE( rootconnect->existUser(user2.name) );

    // user names
    auto allusers = rootconnect->userNames();
    EXPECT_NE( allusers.find(user2.name), allusers.end() );
    EXPECT_NE( allusers.find(otherUser.name), allusers.end() );

    // drop database
    EXPECT_NO_THROW( rootconnect->removeUser(otherUser.name) );
    EXPECT_FALSE( rootconnect->existUser(otherUser.name) );

    allusers = rootconnect->databaseNames();
    EXPECT_EQ( allusers.find(otherUser.name), allusers.end() );
}

TEST_P(RootTestF, testUserGrandUpdate )
{
    auto   rootconnect = GetParam();

    EXPECT_TRUE( rootconnect->existUser(user1.name) );
    EXPECT_FALSE( rootconnect->existUser(user2.name) );

    // create user
    EXPECT_NO_THROW( rootconnect->createUser( user2 ) );
    EXPECT_TRUE( rootconnect->existUser(user2.name) );

    // Modify attributes of an existing user.
    user2.password = "passwd_new";
    user2.access = "rw";
    EXPECT_NO_THROW( rootconnect->updateUser( user2 ));

    // Grant or revoke user access to a database ( "rw", "ro" or "none").
    EXPECT_NO_THROW( rootconnect->grantUserToDataBase(databaseName, user2.name, "ro" ));
    // Retrieves a map contains the databases names as object keys, and the associated privileges
    // for the database as values of all databases the current user can access.
    auto databasegrand = rootconnect->databaseNames( user2.name );
    EXPECT_EQ( databasegrand.size(), 1 );
    EXPECT_EQ( databasegrand.begin()->first , databaseName );
    EXPECT_EQ( databasegrand.begin()->second , "ro" );
}
