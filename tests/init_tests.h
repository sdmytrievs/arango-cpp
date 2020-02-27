#pragma once

#include <string>
#include <vector>
#include <memory>
#include "jsonarango/arangocollection.h"
#include "jsonarango/arangoexception.h"

using arango_connect_t = std::shared_ptr<arangocpp::ArangoDBCollectionAPI>;

struct ConnectionArangoDBParams {
    std::string url;          // ARANGODB_URL
    std::string user;         // ARANGODB_USERNAME
    std::string password;     // ARANGODB_PASSWORD
    std::string database;     // ARANGODB_NAME
    bool        root;         // ROOT_OR_NOT_USER
};

extern const std::vector<ConnectionArangoDBParams> dataTestParams;

extern std::vector<arango_connect_t> connectionTestParams;

//  Function that can be used to replace text using regex.
std::string regexp_replace(const std::string& instr, const std::string& rgx_str, const std::string& replacement );
