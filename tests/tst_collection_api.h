#pragma once

#include <gtest/gtest.h>
//#include <gmock/gmock-matchers.h>

#include "init_tests.h"
#include "jsonarango/arangocollection.h"

using namespace testing;
using namespace arangocpp;


// ConnectionTestF is a test fixture that can be used for all kinds of connection
// tests. You can configure it using the ConnectionTestParams struct.
// Used to test CRUD, create/delete collections, load/delete list of documents for local and remote ArangoDB
class CollectionCRUDTestF : public ::testing::TestWithParam< arango_connect_t >
{

public:

    const std::string collectionName = "test_vertex_API";

protected:

    CollectionCRUDTestF() {}
    virtual ~CollectionCRUDTestF() noexcept override {}


    // Per-test-suite set-up.
    // Called before the first test in this test suite.
    static void SetUpTestSuite()
    {
        try {
            // if root create new db   "test_db_crud"
        } catch(std::exception const& ex) {
            std::cout << "SETUP OF FIXTURE FAILED" << std::endl;
            throw ex;
        }

    }

    // Per-test-suite tear-down.
    // Called after the last test in this test suite.
    static void TearDownTestSuite()
    {
        // if root delete database  "test_db_crud"
    }

    virtual void SetUp() override
    {
        try {
            auto   connect = GetParam();
            //std::cout <<  GetParam()->batchSize() << std::endl;
            connect->createCollection(collectionName, "vertex");
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
        auto   connect = GetParam();
        connect->dropCollection(collectionName);
    }

private:

};

INSTANTIATE_TEST_SUITE_P(BasicConnectionTests, CollectionCRUDTestF,
                         ::testing::ValuesIn(connectionTestParams));

TEST_P(CollectionCRUDTestF, testCollectionsCreateSelect )
{
    auto   connect = GetParam();
    std::string edgeName = "test_Edge";

    EXPECT_TRUE( connect->existCollection(collectionName) );
    EXPECT_FALSE( connect->existCollection(edgeName) );

    // create collection
    EXPECT_NO_THROW( connect->createCollection(edgeName, "edge" ) );
    ASSERT_TRUE( connect->existCollection(edgeName) );

    // collection names
    auto allexist = connect->collectionNames( CollectionTypes::All );
    EXPECT_NE( allexist.find(collectionName), allexist.end() );
    EXPECT_NE( allexist.find(edgeName), allexist.end() );

    auto vertexexist = connect->collectionNames( CollectionTypes::Vertex );
    EXPECT_NE( vertexexist.find(collectionName), vertexexist.end() );
    EXPECT_EQ( vertexexist.find(edgeName), vertexexist.end() );

    auto edgesexist = connect->collectionNames( CollectionTypes::Edge );
    EXPECT_EQ( edgesexist.find(collectionName), edgesexist.end() );
    EXPECT_NE( edgesexist.find(edgeName), edgesexist.end() );

    // drop collection
    EXPECT_NO_THROW( connect->dropCollection(edgeName) );
    EXPECT_FALSE( connect->existCollection(edgeName) );

    allexist = connect->collectionNames( CollectionTypes::All );
    EXPECT_EQ( allexist.find(edgeName), allexist.end() );
}


TEST_P( CollectionCRUDTestF, testCreateDocument )
{
    std::string documentHandle = collectionName+"/eCreate";
    std::string documentData = "{ \"_key\" : \"eCreate\", "
                               "  \"task\" : \"exampleCRUD\" }";
    auto   connect = GetParam();

    EXPECT_FALSE( connect->existsDocument( collectionName, documentHandle));

    auto rkey = connect->createDocument( collectionName, documentData );
    EXPECT_EQ(rkey, documentHandle );
    EXPECT_TRUE( connect->existsDocument( collectionName, documentHandle));

    std::string readDocumentData;
    EXPECT_NO_THROW( connect->readDocument( collectionName, rkey,  readDocumentData) );
    auto delrev = regexp_replace(readDocumentData, rev_regexp, "" );
    EXPECT_EQ( delrev, "{\"_id\":\"test_vertex_API/eCreate\",\"_key\":\"eCreate\",\"task\":\"exampleCRUD\"}" );

    // try use the same key
    EXPECT_THROW( connect->createDocument( collectionName, "{ \"_key\" : \"eCreate\", \"a\" : 1 }" ), arango_exception);
    // error json
    EXPECT_THROW( connect->createDocument( collectionName, "{  \"a\" : 1 " ), arango_exception);
}

TEST_P( CollectionCRUDTestF, testReadDocument )
{
    std::string documentHandle = collectionName+"/eRead";
    std::string documentData = "{ \"_key\" : \"eRead\", "
                               "  \"task\" : \"exampleCRUD\" }";
    auto   connect = GetParam();

    auto rkey = connect->createDocument( collectionName, documentData );
    EXPECT_EQ(rkey, documentHandle );

    std::string readDocumentData;
    EXPECT_NO_THROW( connect->readDocument( collectionName, documentHandle,  readDocumentData) );
    auto delrev = regexp_replace(readDocumentData, rev_regexp, "" );
    EXPECT_EQ( delrev, "{\"_id\":\"test_vertex_API/eRead\",\"_key\":\"eRead\",\"task\":\"exampleCRUD\"}" );

    // try read not exist
    EXPECT_THROW( connect->readDocument( collectionName, collectionName+"/eReadNotExist",  readDocumentData), arango_exception);
}

TEST_P( CollectionCRUDTestF, testDeleteDocument )
{
    std::string documentHandle = collectionName+"/eDelete";
    std::string documentData = "{ \"_key\" : \"eDelete\", "
                               "  \"task\" : \"exampleCRUD\" }";
    auto   connect = GetParam();

    auto rkey = connect->createDocument( collectionName, documentData );
    EXPECT_EQ(rkey, documentHandle );
    EXPECT_TRUE( connect->existsDocument( collectionName, documentHandle));
    EXPECT_NO_THROW( connect->deleteDocument( collectionName, documentHandle ) );
    EXPECT_FALSE( connect->existsDocument( collectionName, documentHandle));

    // error delete not exist
    EXPECT_THROW( connect->deleteDocument( collectionName, collectionName+"/eDelNotExist" ), arango_exception);
    EXPECT_THROW( connect->deleteDocument( "collectionName", documentHandle ), arango_exception);
}

TEST_P( CollectionCRUDTestF, testUpdateDocument )
{
    std::string documentHandle = collectionName+"/eUpdate";
    std::string documentData = "{ \"_key\" : \"eUpdate\", "
                               "  \"task\" : \"exampleCRUD\" }";
    auto   connect = GetParam();

    auto rkey = connect->createDocument( collectionName, documentData );
    EXPECT_EQ(rkey, documentHandle );

    std::string readDocumentData;
    EXPECT_NO_THROW( connect->readDocument( collectionName, rkey,  readDocumentData) );
    auto delrev = regexp_replace(readDocumentData, rev_regexp, "" );
    EXPECT_EQ( delrev, "{\"_id\":\"test_vertex_API/eUpdate\",\"_key\":\"eUpdate\",\"task\":\"exampleCRUD\"}" );

    documentData = "{\"_id\":\"test_vertex_API/eUpdate\",\"_key\":\"eUpdate\",\"a\":1}";
    auto testkey = connect->updateDocument( collectionName, rkey, documentData );
    EXPECT_EQ( testkey, rkey );
    EXPECT_NO_THROW( connect->readDocument( collectionName, rkey,  readDocumentData) );
    delrev = regexp_replace(readDocumentData, rev_regexp, "" );
    EXPECT_EQ( delrev, "{\"_id\":\"test_vertex_API/eUpdate\",\"_key\":\"eUpdate\",\"a\":1}" );

    // try use other key
    EXPECT_THROW( connect->updateDocument( collectionName, collectionName+"/eUpdNotExist", documentData ), arango_exception);
    // error json
    EXPECT_THROW( connect->updateDocument( collectionName, rkey, "{  \"a\" : 1 " ), arango_exception);
}

TEST_P( CollectionCRUDTestF, testDeleteList )
{
    auto   connect = GetParam();

    std::vector<std::string> recKeys;
    // Insert documents to database
    for( int ii=0; ii<3; ii++ )
    {
        ::arangodb::velocypack::Builder builder;
        builder.openObject();
        builder.add("name" , ::arangodb::velocypack::Value(  ii%2 ? "a" : "b" ) );
        builder.add("index" , ::arangodb::velocypack::Value(  ii ) );
        builder.close();

        auto rkey = connect->createDocument( collectionName, builder.toJson() );
        recKeys.push_back(rkey);
    }

    for( const auto& rec: recKeys)
        EXPECT_TRUE( connect->existsDocument( collectionName, rec));

    EXPECT_NO_THROW( connect->removeByKeys( collectionName, recKeys ) );

    for( const auto& rec: recKeys)
        EXPECT_FALSE( connect->existsDocument( collectionName, rec));

}

TEST_P( CollectionCRUDTestF, testLoadList )
{
    auto   connect = GetParam();

    std::vector<std::string> recKeys;
    std::set<std::string> invalues;
    std::vector<std::string> values;
    // Insert documents to database
    for( int ii=0; ii<3; ii++ )
    {
        ::arangodb::velocypack::Builder builder;
        builder.openObject();
        builder.add("name" , ::arangodb::velocypack::Value(  ii%2 ? "a" : "b" ) );
        builder.add("index" , ::arangodb::velocypack::Value(  ii ) );
        builder.close();

        auto data = builder.toJson();
        invalues.insert(data);
        auto rkey = connect->createDocument( collectionName, data );
        recKeys.push_back(rkey);
    }

    arangocpp::FetchingDocumentCallback setfnc = [&invalues]( const std::string& jsondata )
    {
        auto delrev = regexp_replace(jsondata, id_regexp, "" );
        delrev = regexp_replace(delrev, key_regexp, "" );
        delrev = regexp_replace(delrev, rev_regexp, "" );
        EXPECT_NE( invalues.find(delrev), invalues.end());
    };

    // Load by keys list
    connect->lookupByKeys( collectionName, recKeys, setfnc );
}

TEST_P( CollectionCRUDTestF, testSanitizedKey )
{
    std::string documentHandle = collectionName+"/test_7_-:.@()+,$!*'";
    std::string documentData = "{ \"_key\" : \"test_7_-:.@()+,$!*'\", "
                               "  \"task\" : \"exampleCRUD\" }";
    auto   connect = GetParam();

    auto rkey = connect->createDocument( collectionName, documentData );
    EXPECT_EQ(rkey, documentHandle );
    EXPECT_TRUE( connect->existsDocument( collectionName, documentHandle));

    std::string readDocumentData;
    EXPECT_NO_THROW( connect->readDocument( collectionName, documentHandle,  readDocumentData) );

    EXPECT_NO_THROW( connect->deleteDocument( collectionName, documentHandle ) );
    EXPECT_FALSE( connect->existsDocument( collectionName, documentHandle));
}
