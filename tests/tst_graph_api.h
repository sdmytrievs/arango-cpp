#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "init_tests.h"
#include "jsonarango/arangocollection.h"

using namespace testing;
using namespace arangocpp;


// ConnectionTestF is a test fixture that can be used for all kinds of connection
// tests. You can configure it using the ConnectionTestParams struct.
// Used to test ArangoDBGraphAPI - the API for manipulating graphs.
class GraphCRUDTestF : public ::testing::TestWithParam< arango_graph_t >
{

public:

    const std::string graphName = "test_graph_API";
    // Test collection names
    std::string vertex1 = "startVertex";
    std::string vertex2 = "endVertex";
    std::string edge12 = "linkEdge";
    std::string edgeDefinition = "[ {  \"collection\" : \"linkEdge\","
                                 "     \"from\" : [ \"startVertex\" ],"
                                 "     \"to\" :   [ \"endVertex\"  ] } ] ";

protected:

    GraphCRUDTestF() {}
    virtual ~GraphCRUDTestF() noexcept override {}


    virtual void SetUp() override
    {
        try {
            auto   connect = GetParam();
            connect->createGraph( graphName, edgeDefinition );
        }
        catch(std::exception const& ex)
        {
            std::cout << "SETUP OF FIXTURE FAILED" << std::endl;
            throw ex;
        }
    }

    virtual void TearDown() override
    {
        auto   connect = GetParam();
        connect->removeGraph( graphName, true );
    }

private:

};

INSTANTIATE_TEST_SUITE_P(BasicConnectionTests, GraphCRUDTestF,
                         ::testing::ValuesIn(graphTestParams));

TEST_P(GraphCRUDTestF, testCollectionsCreateSelect )
{
    auto   connect = GetParam();
    std::string grName = "links";

    std::string linkDefinition = "[ {  \"collection\" : \"edge\","
                                 "     \"from\" : [ \"vertex1\" ],"
                                 "     \"to\" :   [ \"vertex2\"  ] } ] ";

    EXPECT_TRUE( connect->existGraph(graphName) );
    EXPECT_FALSE( connect->existGraph(grName) );

    // create graph
    EXPECT_NO_THROW( connect->createGraph( grName, linkDefinition ) );
    ASSERT_TRUE( connect->existGraph(grName) );

    // collection names
    auto allexist = connect->graphNames();
    EXPECT_NE( allexist.find(graphName), allexist.end() );
    EXPECT_NE( allexist.find(grName), allexist.end() );


    // drop collection
    EXPECT_NO_THROW( connect->removeGraph(grName, true ) );
    EXPECT_FALSE( connect->existGraph(grName) );

    allexist = connect->graphNames();
    EXPECT_EQ( allexist.find(grName), allexist.end() );
}

/*
TEST_P( GraphCRUDTestF, testCreateDocument )
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

TEST_P( GraphCRUDTestF, testReadDocument )
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

TEST_P( GraphCRUDTestF, testDeleteDocument )
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

TEST_P( GraphCRUDTestF, testUpdateDocument )
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

TEST_P( GraphCRUDTestF, testDeleteVertex )
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

*/
