#pragma once

#include <string>
#include <set>
#include <map>
#include <vector>
#include <memory>
#include <iostream>

namespace arangocpp {

class ArangoDBCollectionAPI;
class ArangoDBUsersAPI;

/// HTTP Interface for User Management
struct ArangoDBUser
{
    /// The name of the user as a string. This is mandatory.
    std::string name;
    /// The user password as a string.
    std::string password;
    /// Database Access  "rw" (read&write) or "ro" ( read only ).
    std::string access;
    /// An optional flag that specifies whether the user is active. If not specified, this will default to true.
    bool active;          // must be true
    /// An optional JSON object with arbitrary extra data about the user.
    std::string extra;


    ArangoDBUser( const std::string& theUser, const std::string& thePasswd,
                  const std::string& theAccess = "rw",
                  bool isactive = true, const std::string&  jsonextra=""):
        name(theUser), password(thePasswd), access(theAccess), active(isactive), extra(jsonextra)
    { }

    ArangoDBUser():
        name("root"), password(""), access("rw"), active(true), extra("")
    { }
};

bool operator!=(const ArangoDBUser& lhs, const ArangoDBUser& rhs);


struct ArangoDBConnection
{
    /// Server URL.
    std::string serverUrl;   // "http://localhost:8529"
    /// The Database Name.
    std::string databaseName;

    /// Server User Data.
    ArangoDBUser user;

    ArangoDBConnection( const std::string& theURL, const std::string& theUser,
                     const std::string& thePasswd, const std::string& theDBname):
        serverUrl(theURL),  databaseName(theDBname), user(theUser, thePasswd)
    {}

    ArangoDBConnection():  serverUrl(local_server_endpoint), databaseName(local_server_database)
    {}

    std::string fullHost() const
    {  return serverUrl+"/_db/"+databaseName; }

    std::string fullURL( const std::string& localpath ) const
    {
        return  fullHost()+localpath;
    }

    bool readonlyDBAccess() const
    {
        return( user.access=="ro");
    }

    /// Full list of edges used in connection query.
    static std::vector<std::string> full_list_of_edges;
    /// Use content type Velocypack on sending requests (fu_content_type_vpack)
    static bool use_velocypack_put;
    /// Use content type Velocypack on getting results
    static bool use_velocypack_get;

    static const char* local_server_endpoint;
    static const char* local_server_username;
    static const char* local_server_password;
    static const char* local_server_database;

    static const char* local_root_username;
    static const char* local_root_password;
    static const char* local_root_database;

    static const char* remote_server_endpoint;
    static const char* remote_server_username;
    static const char* remote_server_password;
    static const char* remote_server_database;
};

bool operator!=(const ArangoDBConnection& lhs, const ArangoDBConnection& rhs);


/// Definition of ArangoDB root API Low Level C++ Driver for ArangoDB.
class ArangoDBRootClient final
{

    /// Connection
    ArangoDBConnection rootData;

    /// ArangoDB database/user data
    std::shared_ptr<ArangoDBUsersAPI> pusers;

public:

    /// Constructor
    ArangoDBRootClient( const ArangoDBConnection& rootconnectData )
    {
        resetDBConnection( rootconnectData );
    }

    /// Reset connections to ArangoDB server
    void resetDBConnection( const ArangoDBConnection& connectData );

    // Database API

    /// Retrieves the list of all existing databases.
    std::set<std::string> databaseNames();

    /// Create Data base dbname with list of existing users if no exist
    /// \param users: Login Names of the users to be accessed.
    void createDatabase( const std::string& dbname, const std::vector<ArangoDBUser>&  users = {} );

    // Users API

    /// Create user
    void createUser( const ArangoDBUser& userdata );
    /// Retrieves a map contains the databases names as object keys, and the associated privileges
    /// for the database as values of all databases the current user can access.
    std::map<std::string,std::string> databaseNames( const std::string&  user );
    /// Fetches data about all users.
    std::set<std::string> userNames();

};

/// ArangoDB connections data from config file.
ArangoDBConnection connectFromConfig( const std::string& cfgfile );
/// ArangoDB root connections data from config file.
ArangoDBConnection rootClientFromConfig( const std::string& cfgfile );
/// Get settings data from json string for root or user
ArangoDBConnection connectFromSettings( const std::string& jsonstr, bool rootdata );

} // namespace arangocpp


