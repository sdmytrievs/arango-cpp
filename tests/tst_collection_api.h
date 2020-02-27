#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "init_tests.h"
#include "jsonarango/arangocollection.h"

using namespace testing;
using namespace arangocpp;

// ConnectionTestF is a test fixture that can be used for all kinds of connection
// tests. You can configure it using the ConnectionTestParams struct.
class CollectionCRUDTestF : public ::testing::TestWithParam< arango_connect_t >
{

public:

 const std::string collectionName = "test_vertex_API1";

protected:

  CollectionCRUDTestF() {}
  virtual ~CollectionCRUDTestF() noexcept override {}


  // Per-test-suite set-up.
  // Called before the first test in this test suite.
    static void SetUpTestSuite()
    {
        try {
            std::cout <<  "GetParam().url   2222" << std::endl;

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

      //auto   connect = GetParam();
      //std::cout <<  GetParam()->batchSize() << std::endl;
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
     //auto   connect = GetParam();
  }

 private:

};

// Add tests exist, create, read, update, delete



INSTANTIATE_TEST_SUITE_P(BasicConnectionTests, CollectionCRUDTestF,
  ::testing::ValuesIn(connectionTestParams));

TEST_P(CollectionCRUDTestF, testCollectionsCreateSelect )
{
  auto   connect = GetParam();
  std::string edgeName = "test_Edge";
  std::string vertexName = "test_Vertex";

  //EXPECT_TRUE( connect->existCollection(vertexName) );
  EXPECT_NO_THROW( connect->createCollection(vertexName, "vertex" ) );

  EXPECT_TRUE( connect->existCollection(vertexName) );
  EXPECT_FALSE( connect->existCollection(edgeName) );

  // create collection
  EXPECT_NO_THROW( connect->createCollection(edgeName, "edge" ) );
  ASSERT_TRUE( connect->existCollection(edgeName) );

  // collection names
  auto allexist = connect->collectionNames( CollectionTypes::All );
  EXPECT_NE( allexist.find(vertexName), allexist.end() );
  EXPECT_NE( allexist.find(edgeName), allexist.end() );

  auto vertexexist = connect->collectionNames( CollectionTypes::Vertex );
  EXPECT_NE( vertexexist.find(vertexName), vertexexist.end() );
  EXPECT_EQ( vertexexist.find(edgeName), vertexexist.end() );

  auto edgesexist = connect->collectionNames( CollectionTypes::Edge );
  EXPECT_EQ( edgesexist.find(vertexName), edgesexist.end() );
  EXPECT_NE( edgesexist.find(edgeName), edgesexist.end() );

  // drop collection
  EXPECT_NO_THROW( connect->dropCollection(edgeName) );
  EXPECT_FALSE( connect->existCollection(edgeName) );

  EXPECT_NO_THROW( connect->dropCollection(vertexName) );
  EXPECT_FALSE( connect->existCollection(vertexName) );

  allexist = connect->collectionNames( CollectionTypes::All );
  EXPECT_EQ( allexist.find(edgeName), allexist.end() );
  EXPECT_EQ( allexist.find(vertexName), allexist.end() );
}


TEST_P(CollectionCRUDTestF, acceptsEekyWords2)
{
    auto   connect = GetParam();

    //connect->createCollection(collectionName, "vertex");

    EXPECT_TRUE(true);
    //connect->dropCollection(collectionName);
    //auto allexist = connect->collectionNames( CollectionTypes::All );
}

