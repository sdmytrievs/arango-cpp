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

    std::string linkDefinition = "[ {  \"collection\" : \"edge12\","
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

    // Adds a vertex collection to the set of collections of the graph.
    EXPECT_NO_THROW( connect->addVertexGraph(grName, "vertex3" ));

    // Adds an additional edge definition to the graph.
    std::string linkDefinition2 = "{  \"collection\" : \"edge21\","
                                 "     \"from\" : [ \"vertex3\" ],"
                                 "     \"to\" :   [ \"vertex4\"  ] }";
    EXPECT_NO_THROW( connect->addEdgeGraph( grName, linkDefinition2 ) );


    // Fetches all (edges/vertexes) collections from the graph.
    auto vertexnames =  connect->graphCollectionNames( grName, "vertex" );
    EXPECT_EQ(vertexnames, std::set<std::string>({"vertex1", "vertex2", "vertex3", "vertex4"}) );

    auto edgenames =  connect->graphCollectionNames( grName, "edge" );
    EXPECT_EQ(edgenames, std::set<std::string>({"edge12", "edge21"}) );

    // drop collection
    EXPECT_NO_THROW( connect->removeGraph(grName, true ) );
    EXPECT_FALSE( connect->existGraph(grName) );

    allexist = connect->graphNames();
    EXPECT_EQ( allexist.find(grName), allexist.end() );
}

TEST_P( GraphCRUDTestF, testCreatVertexEdge )
{
    auto   connect = GetParam();

    std::string record1 = "{\"_key\":\"v_from\",\"a\":1,\"name\":\"vertex\"}";
    auto id1 = connect->createRecord( graphName, "vertex", vertex1, record1 );
    std::string record2 = "{\"_key\":\"v_to\",\"b\":2,\"name\":\"vertex\"}";
    auto id2 = connect->createRecord( graphName, "vertex", vertex2, record2 );

    std::string edgerecord = "{\"_from\":\"startVertex/v_from\",\"_key\":\"g_11\",\"_to\":\"endVertex/v_to\",\"ab\":12}";
    auto id3 = connect->createRecord( graphName, "edge", edge12, edgerecord );

    std::string record;
    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex1, id1, record ) );
    auto delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, record1 );

    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex2, id2, record ) );
    delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, record2 );

    EXPECT_NO_THROW( connect->readRecord( graphName, "edge", edge12, id3, record ) );
    delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, edgerecord );
}


TEST_P( GraphCRUDTestF, testReadVertexEdge )
{
    auto   connect = GetParam();

    std::string record1 = "{\"_key\":\"v_from\",\"a\":1,\"name\":\"vertex\"}";
    auto id1 = connect->createRecord( graphName, "vertex", vertex1, record1 );
    std::string record2 = "{\"_key\":\"v_to\",\"b\":2,\"name\":\"vertex\"}";
    auto id2 = connect->createRecord( graphName, "vertex", vertex2, record2 );
    std::string edgerecord = "{\"_from\":\"startVertex/v_from\",\"_key\":\"g_11\",\"_to\":\"endVertex/v_to\",\"ab\":12}";
    auto id3 = connect->createRecord( graphName, "edge", edge12, edgerecord );

    std::string record;
    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex1, id1, record ) );
    auto delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, record1 );

    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex2, id2, record ) );
    delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, record2 );

    EXPECT_NO_THROW( connect->readRecord( graphName, "edge", edge12, id3, record ) );
    delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, edgerecord );

    EXPECT_THROW( connect->readRecord( graphName, "vertex", vertex2, vertex2+"/not_exist", record ), arango_exception );
    EXPECT_THROW( connect->readRecord( graphName, "vertex", "not_exist", id2, record ), arango_exception );

    EXPECT_THROW( connect->readRecord( graphName, "edge", edge12, edge12+"/not_exist", record ), arango_exception );
    EXPECT_THROW( connect->readRecord( graphName, "edge", "not_exist", id3, record ), arango_exception );
}


TEST_P( GraphCRUDTestF, testDeleteVertex )
{
    auto   connect = GetParam();

    std::string record1 = "{\"_key\":\"v_from\",\"a\":1,\"name\":\"vertex\"}";
    auto id1 = connect->createRecord( graphName, "vertex", vertex1, record1 );
    std::string record2 = "{\"_key\":\"v_to\",\"b\":2,\"name\":\"vertex\"}";
    auto id2 = connect->createRecord( graphName, "vertex", vertex2, record2 );
    std::string edgerecord = "{\"_from\":\"startVertex/v_from\",\"_key\":\"g_11\",\"_to\":\"endVertex/v_to\",\"ab\":12}";
    auto id3 = connect->createRecord( graphName, "edge", edge12, edgerecord );

    std::string record;
    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex1, id1, record ) );
    auto delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, record1 );

    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex2, id2, record ) );
    delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, record2 );

    EXPECT_NO_THROW( connect->readRecord( graphName, "edge", edge12, id3, record ) );
    delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, edgerecord );

    EXPECT_NO_THROW( connect->deleteRecord( graphName, "vertex", vertex1, id1 ));

    EXPECT_THROW( connect->readRecord( graphName, "vertex", vertex1, id1, record ), arango_exception );
    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex2, id2, record ) );

    EXPECT_THROW( connect->readRecord( graphName, "edge", edge12, id3, record ), arango_exception );
}

TEST_P( GraphCRUDTestF, testDeleteEdge )
{
    auto   connect = GetParam();

    std::string record1 = "{\"_key\":\"v_from\",\"a\":1,\"name\":\"vertex\"}";
    auto id1 = connect->createRecord( graphName, "vertex", vertex1, record1 );
    std::string record2 = "{\"_key\":\"v_to\",\"b\":2,\"name\":\"vertex\"}";
    auto id2 = connect->createRecord( graphName, "vertex", vertex2, record2 );
    std::string edgerecord = "{\"_from\":\"startVertex/v_from\",\"_key\":\"g_11\",\"_to\":\"endVertex/v_to\",\"ab\":12}";
    auto id3 = connect->createRecord( graphName, "edge", edge12, edgerecord );

    std::string record;
    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex1, id1, record ) );
    auto delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, record1 );

    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex2, id2, record ) );
    delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, record2 );

    EXPECT_NO_THROW( connect->readRecord( graphName, "edge", edge12, id3, record ) );
    delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, edgerecord );

    EXPECT_NO_THROW( connect->deleteRecord( graphName, "edge", edge12, id3 ));

    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex1, id1, record ) );
    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex2, id2, record ) );

    EXPECT_THROW( connect->readRecord( graphName, "edge", edge12, id3, record ), arango_exception );
}


TEST_P( GraphCRUDTestF, testUpdateVertexEdge )
{
    auto   connect = GetParam();

    std::string record1 = "{\"_key\":\"v_from\",\"a\":1,\"name\":\"vertex\"}";
    auto id1 = connect->createRecord( graphName, "vertex", vertex1, record1 );
    std::string record2 = "{\"_key\":\"v_to\",\"b\":2,\"name\":\"vertex\"}";
    auto id2 = connect->createRecord( graphName, "vertex", vertex2, record2 );
    std::string edgerecord = "{\"_from\":\"startVertex/v_from\",\"_key\":\"g_11\",\"_to\":\"endVertex/v_to\",\"ab\":12}";
    auto id3 = connect->createRecord( graphName, "edge", edge12, edgerecord );

    std::string record;
    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex1, id1, record ) );
    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex2, id2, record ) );
    EXPECT_NO_THROW( connect->readRecord( graphName, "edge", edge12, id3, record ) );

    std::string record11 = "{\"_key\":\"v_from\",\"c\":3,\"name\":\"vertex\"}";
    EXPECT_NO_THROW( connect->updateRecord( graphName, "vertex", vertex1, id1, record11 ));

    EXPECT_NO_THROW( connect->readRecord( graphName, "vertex", vertex1, id1, record ) );
    auto delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, record11 );
    EXPECT_NE( delrev, record1 );

    std::string edgerecord2 = "{\"_from\":\"startVertex/v_from\",\"_key\":\"g_11\",\"_to\":\"endVertex/v_to\",\"abc\":12}";
    EXPECT_NO_THROW( connect->updateRecord( graphName, "edge", edge12, id3, edgerecord2 ));

    EXPECT_NO_THROW( connect->readRecord( graphName, "edge", edge12, id3, record ) );
    delrev = regexp_replace(record, rev_regexp, "" );
    delrev = regexp_replace(delrev, id_regexp, "" );
    EXPECT_EQ( delrev, edgerecord2 );
    EXPECT_NE( delrev, edgerecord );
}
