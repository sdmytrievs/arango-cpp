#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "init_tests.h"
#include "jsonarango/arangocollection.h"

using namespace testing;
using namespace arangocpp;

// ConnectionTestF is a test fixture that can be used for all kinds of connection
// tests. You can configure it using the ConnectionTestParams struct.
class CollectionCRUDTestF : public ::testing::TestWithParam<ConnectionArangoDBParams> {

 public:

 protected:

  CollectionCRUDTestF() {}
  virtual ~CollectionCRUDTestF() noexcept override {}


  // Per-test-suite set-up.
  // Called before the first test in this test suite.
    static void SetUpTestSuite()
    {
        try {
            std::cout <<  GetParam().url << std::endl;

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


  virtual void SetUp() override {
    try {
      // Set connection parameters
      // create collection  "test_crud"
      // add one record
    } catch(std::exception const& ex) {
      std::cout << "SETUP OF FIXTURE FAILED" << std::endl;
      throw ex;
    }
  }

  virtual void TearDown() override {
     // delete collection  "test_crud"
  }

 private:

};

// Add tests exist, create, read, update, delete


INSTANTIATE_TEST_SUITE_P(BasicConnectionTests, CollectionCRUDTestF,
  ::testing::ValuesIn(connectionTestParams));
