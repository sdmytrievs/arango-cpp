#pragma once

#include "jsonarango/arangocollection.h"

namespace arangocpp {

/// API to Create/Delete ArangoDB users and databases
/// Information about root into connect_data
/// You need permission to the _system database in order to execute this REST call.
class ArangoDBUsersAPI : public ArangoDBAPIBase
{

public:

    ///  Constructor
    explicit ArangoDBUsersAPI( const ArangoDBConnection& rootData ):
        ArangoDBAPIBase(rootData)
    { }

    // Data base functions

    /// Retrieves the list of all existing Databases.
    std::set<std::string> databaseNames();

    /// Test exist Database.
    bool existDatabase( const std::string& dbname );

    /// Create Database dbname with list of existing users.
    /// \param users: Login Names of the users to be accessed
    /// https://docs.arangodb.com/3.1/HTTP/Database/DatabaseManagement.html
    ///  Each user object can contain the following attributes:
    ///   username: Loginname of the user to be created
    ///   passwd: The user password as a string. If not specified, it will default to an empty string.
    ///   active: A flag indicating whether the user account should be activated or not. The default value is true.
    ///   extra: A JSON string with extra user information.
    void createDatabase( const std::string& dbname, const std::vector<ArangoDBUser>&  users = {} );

    /// Remove Database.
    void removeDatabase( const std::string& dbname );

    // Users API

    /// Fetches data about the specified user. You can fetch information about yourself
    /// or you need the Administrate server access level in order to execute this REST call.
    bool existUser(const std::string &username);

    /// Create user.
    void createUser( const ArangoDBUser& userdata );
    /// Modify attributes of an existing user.
    void updateUser( const ArangoDBUser& userdata );
    /// Remove user.
    void removeUser( const std::string& username );

    /// Grant or revoke user access to a database ( "rw", "ro" or "none").
    void grantUserToDataBase(const std::string& dbname, const std::string& username, const std::string& grand = "rw" );

    /// Retrieves a map contains the databases names as object keys, and the associated privileges
    /// for the database as values of all databases the current user can access.
    std::map<std::string,std::string> databaseNames( const std::string&  user );

    /// Fetches data about all users.
    std::set<std::string> userNames();

protected:

    std::unique_ptr<HttpMessage> sendREQUEST(std::unique_ptr<HttpMessage> rq ) override;

};


} // namespace arangocpp
