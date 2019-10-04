#pragma once

#include <string>
#include <vector>


struct ConnectionArangoDBParams {
    std::string url;          // ARANGODB_URL
    std::string user;         // ARANGODB_USERNAME
    std::string password;     // ARANGODB_PASSWORD
    std::string database;     // ARANGODB_NAME
    bool        root;         // ROOT_OR_NOT_USER
};

extern const std::vector<ConnectionArangoDBParams> connectionTestParams;
