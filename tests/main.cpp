#include "init_tests.h"
#include "tst_arango.h"
#include "tst_crud.h"


#include <gtest/gtest.h>

// https://doc.qt.io/qtcreator/creator-autotest.html
// https://stackoverflow.com/questions/39574360/google-testing-framework-and-qt
// https://stackoverflow.com/questions/49756620/including-google-tests-in-qt-project

const std::vector<ConnectionArangoDBParams> connectionTestParams =
{
    { "http://localhost:8529", "root", "", "_system", true }
//    { "https://db.thermohub.net", "adminrem", "Administrator@Remote-ThermoHub-Server", "hub_test", false },
};

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
