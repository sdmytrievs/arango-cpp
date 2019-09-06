#ifndef ARANGODBUSERS_H
#define ARANGODBUSERS_H

#include "arangocollection.h"

namespace jsonio { namespace arangodb {

/// API to Create/Delete ArangoDB users and databases
/// Information about root into _connect_data
/// You need permission to the _system database in order to execute this REST call.
class ArangoDBUsersAPI : public ArangoDBAPIBase
{

protected:

    virtual std::unique_ptr<arangodb::HttpMessage> sendRequest(std::unique_ptr<arangodb::HttpMessage> rq );

public:

    ///  Constructor
    explicit ArangoDBUsersAPI( const jsonio::ArangoDBConnect& rootData ):
        ArangoDBAPIBase(rootData)
    { }

    ///  Destructor
    virtual ~ArangoDBUsersAPI()
    { }

    // Data base functions

    /// Retrieves the list of all existing databases
    std::set<std::string> getDatabaseNames();
    /// Test exist database
    bool ExistDatabase( const std::string& dbname );
    /// Create Data base dbname with list of existing users
    /// \param users: Login Names of the users to be accessed
    /// https://docs.arangodb.com/3.1/HTTP/Database/DatabaseManagement.html
    ///  Each user object can contain the following attributes:
    ///   username: Loginname of the user to be created
    ///   passwd: The user password as a string. If not specified, it will default to an empty string.
    ///   active: A flag indicating whether the user account should be activated or not. The default value is true.
    ///   extra: A JSON object with extra user information.
    void CreateDatabase( const std::string& dbname, const std::vector<ArangoDBUser>&  users = {} );
    /// Remove Data base
    void RemoveDatabase( const std::string& dbname );

    // Users API

    /// Create user
    void CreateUser( const ArangoDBUser& userdata );
    /// Modify attributes of an existing user
    void UpdateUser( const ArangoDBUser& userdata );
    /// Remove user
    void RemoveUser( const std::string& username );

    /// Grant or revoke user access to a database ( "rw", "ro" or "none")
    void GrantUserToDataBase(const std::string& dbname, const std::string& username, const std::string& grand = "rw" );
    /// Retrieves a map contains the databases names as object keys, and the associated privileges
    /// for the database as values of all databases the current user can access
    std::map<std::string,std::string> getDatabaseNames( const std::string&  user );

    /// Fetches data about all users
    std::set<std::string> getUserNames();
};


} }

#endif // ARANGODBUSERS_H
