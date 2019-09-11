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
    std::string name; //  "adminrem"
    /// The user password as a string.
    std::string password;  // "Administrator@Remote-ThermoHub-Server"
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


struct ArangoDBConnect
{
    /// Server URL.
    std::string serverUrl;   // "http://localhost:8529"
    /// The Database Name.
    std::string databaseName; //  "hub_test"

    /// Server User Data.
    ArangoDBUser user;

    ArangoDBConnect( const std::string& theURL, const std::string& theUser,
                     const std::string& thePasswd, const std::string& theDBname):
        serverUrl(theURL),  databaseName(theDBname), user(theUser, thePasswd)
    {}

    ArangoDBConnect():  serverUrl("http://localhost:8529"), databaseName("_system")
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

    void getFromSettings(const std::string& group, bool rootdata = false );
    /// Full list of edges used in connection query.
    static std::vector<std::string> full_list_of_edges;
    /// Use content type Velocypack on sending requests (fu_content_type_vpack)
    static bool use_velocypack_put;
    /// Use content type Velocypack on getting results
    static bool use_velocypack_get;

};

bool operator!=(const ArangoDBConnect& lhs, const ArangoDBConnect& rhs);


/// Definition of ArangoDB root API Low Level C++ Driver for ArangoDB.
class ArangoDBRootClient final
{

    /// Connection
    ArangoDBConnect rootData;

    /// ArangoDB database/user data
    std::shared_ptr<ArangoDBUsersAPI> pusers;

public:

    /// Constructor
    ArangoDBRootClient( const ArangoDBConnect& rootconnectData )
    {
        resetDBConnection( rootconnectData );
    }

    /// Reset connections to ArangoDB server
    void resetDBConnection( const ArangoDBConnect& connectData );

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
ArangoDBConnect connectFromConfig( const std::string& cfgfile );
/// ArangoDB root connections data from config file.
ArangoDBRootClient rootClientFromConfig( const std::string& cfgfile );

} // namespace arangocpp


