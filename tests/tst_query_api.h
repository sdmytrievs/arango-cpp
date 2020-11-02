#pragma once

#include <gtest/gtest.h>
//#include <gmock/gmock-matchers.h>

#include "init_tests.h"
#include "jsonarango/arangocollection.h"

using namespace testing;
using namespace arangocpp;


// ConnectionTestF is a test fixture that can be used for all kinds of connection
// tests. You can configure it using the ConnectionTestParams struct.
// Used to test query  documents for local and remote ArangoDB
class CollectionQueriesTestF : public ::testing::TestWithParam< arango_connect_t >
{

public:

    // Test collection name
    const std::string collectionName = "test_query_API";
    // Select from number of documents
    const int numOfDocuments =  10;

protected:

    std::vector<std::string> doc_keys;

    CollectionQueriesTestF() {}
    virtual ~CollectionQueriesTestF() noexcept override {}


    virtual void SetUp() override
    {
        try {
            auto   connect = GetParam();
            connect->createCollection(collectionName, "vertex");
            // Insert documents to database
            for( int ii=0; ii<numOfDocuments; ii++ )
            {
                ::arangodb::velocypack::Builder builder;
                builder.openObject();
                builder.add("name" , ::arangodb::velocypack::Value(  ii%2 ? "a" : "b" ) );
                builder.add("index" , ::arangodb::velocypack::Value(  ii ) );
                builder.close();

                auto rkey = connect->createDocument( collectionName, builder.toJson() );
                doc_keys.push_back(rkey);
            }
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
        EXPECT_NO_THROW( connect->removeByKeys( collectionName, doc_keys ) );
        connect->dropCollection(collectionName);
    }

private:

};

INSTANTIATE_TEST_SUITE_P(BasicConnectionTests, CollectionQueriesTestF,
                         ::testing::ValuesIn(connectionTestParams));

TEST_P(CollectionQueriesTestF, testQuerySelectAll )
{
    auto   connect = GetParam();

    arangocpp::ArangoDBQuery    allquery( arangocpp::ArangoDBQuery::All );
    auto doc_json_values = connect->selectQuery( collectionName, allquery );

    EXPECT_EQ( doc_json_values.size(), numOfDocuments );

    for( const auto& doc: doc_json_values)
    {
      auto data = ::arangodb::velocypack::Parser::fromJson( doc );
      EXPECT_TRUE( data->slice().hasKey("name") );
      EXPECT_TRUE( data->slice().hasKey("index") );
      EXPECT_TRUE( data->slice().get("name").toString() == "a" ||
                   data->slice().get("name").toString() == "b" );
    }
}


TEST_P(CollectionQueriesTestF, testSelectbyTemplate )
{
    auto   connect = GetParam();

    arangocpp::ArangoDBQuery    templatequery( "{ \"name\" : \"a\" }", arangocpp::ArangoDBQuery::Template );

    auto doc_json_values = connect->selectQuery( collectionName, templatequery );

    EXPECT_EQ( doc_json_values.size(), numOfDocuments/2 );

    for( const auto& doc: doc_json_values)
    {
      auto data = ::arangodb::velocypack::Parser::fromJson( doc );
      EXPECT_TRUE( data->slice().get("name").toString() == "a" );
    }
}

TEST_P(CollectionQueriesTestF, testSelectAQL )
{
    auto   connect = GetParam();

    std::string aql = "FOR u IN " + collectionName +
            "\nFILTER u.name == \"a\" \n"
            "RETURN { \"_id\": u._id, \"name\": u.name, \"index\": u.index }";
    arangocpp::ArangoDBQuery    aqlquery( aql, arangocpp::ArangoDBQuery::AQL );

    auto doc_json_values = connect->selectQuery( collectionName, aqlquery );

    EXPECT_EQ( doc_json_values.size(), numOfDocuments/2 );

    for( const auto& doc: doc_json_values)
    {
      auto data = ::arangodb::velocypack::Parser::fromJson( doc );
      EXPECT_TRUE( data->slice().get("name").toString() == "a" );
    }
}


TEST_P(CollectionQueriesTestF, testRemoveByTemplate )
{
    auto   connect = GetParam();

    connect->removeByTemplate( collectionName, "{ \"name\" : \"a\" }" );

    arangocpp::ArangoDBQuery    allquery( arangocpp::ArangoDBQuery::All );
    auto doc_json_values = connect->selectQuery( collectionName, allquery );

    EXPECT_EQ( doc_json_values.size(), numOfDocuments/2 );

    for( const auto& doc: doc_json_values)
    {
      auto data = ::arangodb::velocypack::Parser::fromJson( doc );
      EXPECT_TRUE( data->slice().get("name").toString() == "b" );
    }
}

TEST_P(CollectionQueriesTestF, testSelectAll )
{
    auto   connect = GetParam();
    auto doc_json_values = connect->selectAll( collectionName, {} );

    EXPECT_EQ( doc_json_values.size(), numOfDocuments );

    for( const auto& doc: doc_json_values)
    {
      auto data = ::arangodb::velocypack::Parser::fromJson( doc );
      EXPECT_TRUE( data->slice().hasKey("name") );
      EXPECT_TRUE( data->slice().hasKey("index") );
    }

    doc_json_values = connect->selectAll( collectionName, { { "_id", "_id" }, { "index", "index" } } );

    EXPECT_EQ( doc_json_values.size(), numOfDocuments );

    for( const auto& doc: doc_json_values)
    {
      auto data = ::arangodb::velocypack::Parser::fromJson( doc );
      EXPECT_FALSE( data->slice().hasKey("name") );
      EXPECT_TRUE( data->slice().hasKey("index") );
    }
}


TEST_P(CollectionQueriesTestF, testCollectQuery )
{
    std::vector<std::string> doc_json_values;
    auto   connect = GetParam();

    connect->collectQuery( collectionName,"name", doc_json_values );
    EXPECT_EQ( doc_json_values.size(), 2 );
    EXPECT_TRUE( ( doc_json_values[0] == "\"a\"" && doc_json_values[1] == "\"b\"" ) ||
                 ( doc_json_values[1] == "\"a\"" && doc_json_values[0] == "\"b\"" ) );

    doc_json_values.clear();
    connect->collectQuery( collectionName,"index", doc_json_values );

    EXPECT_EQ( doc_json_values.size(), numOfDocuments );
}

TEST_P(CollectionQueriesTestF, testQueryBindVars )
{
    auto   connect = GetParam();

    std::string aql = "FOR u IN " + collectionName +
            "\nFILTER u.index IN @ndxs  \n"
            "RETURN u";
    arangocpp::ArangoDBQuery    aqlquery( aql, arangocpp::ArangoDBQuery::AQL );
    aqlquery.setBindVars("{ \"ndxs\": [1,2,3] }");

    auto doc_json_values = connect->selectQuery( collectionName, aqlquery );

    EXPECT_EQ( doc_json_values.size(), 3 );
}


TEST_P(CollectionQueriesTestF, testQueryOptions )
{
    auto   connect = GetParam();

    std::string aql = "FOR u IN " + collectionName +
            "\nFILTER u.name == \"a\" \n"
            "RETURN u";
    std::string options = "{ \"maxPlans\" : 1, "
                          "  \"optimizer\" : { \"rules\" : [ \"-all\", \"+remove-unnecessary-filters\" ]  } } ";
    arangocpp::ArangoDBQuery    aqlquery( aql, arangocpp::ArangoDBQuery::AQL );
    aqlquery.setOptions(options);

    auto doc_json_values = connect->selectQuery( collectionName, aqlquery );

    EXPECT_EQ( doc_json_values.size(), numOfDocuments/2 );

    for( const auto& doc: doc_json_values)
    {
      auto data = ::arangodb::velocypack::Parser::fromJson( doc );
      EXPECT_TRUE( data->slice().get("name").toString() == "a" );
    }
}

TEST_P(CollectionQueriesTestF, testSelectInOutEdges )
{
    auto   connect = GetParam();

    std::string edge_name = "edge_query_API";
    std::vector<std::string> edge_keys;
    connect->createCollection(edge_name, "edge");
    // Insert documents to database
    for( size_t ii=0; ii<doc_keys.size()-1; ii++ )
    {
        ::arangodb::velocypack::Builder builder;
        builder.openObject();
        builder.add("_to" , ::arangodb::velocypack::Value(  doc_keys[ii] ) );
        builder.add("_from" , ::arangodb::velocypack::Value(  doc_keys[ii+1] ) );
        builder.add("property" , ::arangodb::velocypack::Value(  ii ) );
        builder.close();

        auto rkey = connect->createDocument( edge_name, builder.toJson() );
        edge_keys.push_back(rkey);
    }

    std::string queryJson = "{ \"startVertex\": \""
            + doc_keys[1]  + "\", \"edgeCollections\": \"\" }";

    arangocpp::ArangoDBQuery    query_to( queryJson, arangocpp::ArangoDBQuery::EdgesTo );
    auto edge_to_values = connect->selectQuery( edge_name, query_to );
    EXPECT_EQ( edge_to_values.size(), 1 );
    auto data = ::arangodb::velocypack::Parser::fromJson( edge_to_values[0] );
    EXPECT_TRUE( data->slice().get("property").toString()  == "1" );

    arangocpp::ArangoDBQuery    query_from( queryJson, arangocpp::ArangoDBQuery::EdgesFrom );
    edge_to_values = connect->selectQuery( edge_name, query_from );
    EXPECT_EQ( edge_to_values.size(), 1 );
    data = ::arangodb::velocypack::Parser::fromJson( edge_to_values[0] );
    EXPECT_TRUE( data->slice().get("property").toString()  == "0" );

    arangocpp::ArangoDBQuery    query_all( queryJson, arangocpp::ArangoDBQuery::EdgesAll );
    edge_to_values = connect->selectQuery( edge_name, query_all );
    EXPECT_EQ( edge_to_values.size(), 2 );
    data = ::arangodb::velocypack::Parser::fromJson( edge_to_values[0] );
    EXPECT_TRUE( data->slice().get("property").toString()  == "0" ||
                 data->slice().get("property").toString()  == "1" );


    ArangoDBConnection::full_list_of_edges = {edge_name};
    ArangoDBQuery qr_to = connect->queryEdgesToFrom( arangocpp::ArangoDBQuery::EdgesAll, doc_keys[1], "" );
    EXPECT_EQ( qr_to.queryString(), "FOR v,e IN 1..1 ANY '"+doc_keys[1]+"' \nedge_query_API\nRETURN DISTINCT e " );

    EXPECT_NO_THROW( connect->removeByKeys( edge_name, edge_keys ) );
    connect->dropCollection(edge_name);
}
