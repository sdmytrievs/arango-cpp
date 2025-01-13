#pragma once

#include <string>
#include <vector>
#include <memory>
#include "arango-cpp/arangocollection.h"
#include "arango-cpp/arangograph.h"
#include "arango-cpp/arangoexception.h"


using arango_connect_t = std::shared_ptr<arangocpp::ArangoDBCollectionAPI>;
using arango_graph_t = std::shared_ptr<arangocpp::ArangoDBGraphAPI>;
using arango_root_t = std::shared_ptr<arangocpp::ArangoDBUsersAPI>;

struct ConnectionArangoDBParams {
    std::string url;          // ARANGODB_URL
    std::string user;         // ARANGODB_USERNAME
    std::string password;     // ARANGODB_PASSWORD
    std::string database;     // ARANGODB_NAME
    bool        root;         // ROOT_OR_NOT_USER
};

extern const std::vector<ConnectionArangoDBParams> dataTestParams;

extern std::vector<arango_connect_t> connectionTestParams;
extern std::vector<arango_graph_t> graphTestParams;
extern std::vector<arango_root_t> rootTestParams;

const char* rev_regexp = "(\"_rev\":\"[^\"]*\",)";
const char* id_regexp = "(\"_id\":\"[^\"]*\",)";
const char* key_regexp = "(\"_key\":\"[^\"]*\",)";

//  Function that can be used to replace text using regex.
std::string regexp_replace(const std::string& instr, const std::string& rgx_str, const std::string& replacement );
