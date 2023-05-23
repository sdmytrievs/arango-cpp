
#include <regex>
#include <gtest/gtest.h>

#include "init_tests.h"
#include "tst_arango.h"
#include "tst_collection_api.h"
#include "tst_query_api.h"
#include "tst_graph_api.h"
#ifdef TestLocalServer
 #include "tst_root_api.h"
#endif
// https://doc.qt.io/qtcreator/creator-autotest.html
// https://stackoverflow.com/questions/39574360/google-testing-framework-and-qt
// https://stackoverflow.com/questions/49756620/including-google-tests-in-qt-project

const std::vector<ConnectionArangoDBParams> dataTestParams =
{
#ifdef TestLocalServer
    { "http://localhost:8529", "root", "", "test_db_api", false },
#endif

#ifdef TestRemoteServer
    { "https://db.thermohub.net", "test_api_user", "TestApiUser@Remote-ThermoHub-Server", "test_db_api", false },
#endif
};

const std::vector<ConnectionArangoDBParams> dataRootTestParams =
{
#ifdef TestLocalServer
    { "http://localhost:8529", "root", "", "_system", true }
#endif
};


std::vector<arango_connect_t> connectionTestParams;
std::vector<arango_graph_t> graphTestParams;
std::vector<arango_root_t> rootTestParams;

int main(int argc, char *argv[])
{
    // Init params (could be from config file)
    for( auto& data: dataTestParams )
    {
        connectionTestParams.push_back( std::make_shared<arangocpp::ArangoDBCollectionAPI>(
                                        arangocpp::ArangoDBConnection( data.url, data.user, data.password, data.database ) ));

        graphTestParams.push_back( std::make_shared<arangocpp::ArangoDBGraphAPI>(
                                        arangocpp::ArangoDBConnection( data.url, data.user, data.password, data.database ) ));
    }

    // Init params (could be from config file)
    for( auto& data: dataRootTestParams )
    {
        rootTestParams.push_back( std::make_shared<arangocpp::ArangoDBUsersAPI>(
                                        arangocpp::ArangoDBConnection( data.url, data.user, data.password, data.database ) ));
        rootTestParams.back()->createDatabase("test_db_api");  // create if not exist
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

//  Function that can be used to replace text using regex.
std::string regexp_replace(const std::string& instr, const std::string& rgx_str, const std::string& replacement )
{
   std::regex re(rgx_str);
   std::string output_str = regex_replace(instr, re, replacement);
   return output_str;
}
