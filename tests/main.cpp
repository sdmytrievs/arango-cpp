

#include <gtest/gtest.h>

#include "init_tests.h"
#include "tst_arango.h"
#include "tst_collection_api.h"

// https://doc.qt.io/qtcreator/creator-autotest.html
// https://stackoverflow.com/questions/39574360/google-testing-framework-and-qt
// https://stackoverflow.com/questions/49756620/including-google-tests-in-qt-project

const std::vector<ConnectionArangoDBParams> dataTestParams =
{
    { "http://localhost:8529", "root", "", "test_database", false }
//    { "http://localhost:8529", "root", "", "_system", true },
//    { "https://db.thermohub.net", "__put_here_the_user_name__", "__put_here_the_remote_password__", "hub_test", false },
};


std::vector<arango_connect_t> connectionTestParams;

int main(int argc, char *argv[])
{
    // Init params (could be from config file)
    for( auto& data: dataTestParams )
    {
        connectionTestParams.push_back( std::make_shared<arangocpp::ArangoDBCollectionAPI>(
                                        arangocpp::ArangoDBConnection( data.url, data.user, data.password, data.database ) ));
    }

//connectionTestParams[0]->dropCollection("test_vertex_API");
//connectionTestParams[0]->dropCollection("test_Vertex");
//connectionTestParams[0]->dropCollection("test_Vertex1");
//return 0;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
