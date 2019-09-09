//  This is JSONIO library+API (https://bitbucket.org/gems4/jsonio)
//
/// \file dbarango.h
/// Declarations of class
/// TArangoDBClient - DB driver for working with ArangoDB
//
// JSONIO is a C++ library and API aimed at implementing the interfaces
// for exchanging the structured data between NoSQL database backends,
// JSON/YAML/XML files, and client-server RPC (remote procedure calls).
//
// Copyright (c) 2015-2016 Svetlana Dmytriieva (svd@ciklum.com) and
//   Dmitrii Kulik (dmitrii.kulik@psi.ch)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU (Lesser) General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// JSONIO depends on the following open-source software products:
// Apache Thrift (https://thrift.apache.org); Pugixml (http://pugixml.org);
// YAML-CPP (https://github.com/jbeder/yaml-cpp); ArangoDB (https://github.com/arangodb/fuerte).
//

#ifndef TDBARANGO_H
#define TDBARANGO_H

#include <string>
#include <set>
#include <map>
#include <vector>
#include <memory>

namespace jsonio {

namespace arangodb {
class ArangoDBCollectionAPI;
class ArangoDBUsersAPI;
}

/// HTTP Interface for User Management
struct ArangoDBUser
{
    /// The name of the user as a string. This is mandatory
    std::string name; //  "adminrem"
    /// The user password as a string
    std::string password;  // "Administrator@Remote-ThermoHub-Server"
    /// Database Access  "rw" or "ro"
    std::string access;
    /// An optional flag that specifies whether the user is active. If not specified, this will default to true
    bool active;          // must be true
    /// An optional JSON object with arbitrary extra data about the user
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
    /// Server URL
    std::string serverUrl;   // "http://localhost:8529"
    /// The Database Name
    std::string databaseName; //  "hub_test"

    /// Server User Data
    ArangoDBUser user;

    ArangoDBConnect( const std::string& theURL, const std::string& theUser,
               const std::string& thePasswd, const std::string& theDBname):
       serverUrl(theURL),  databaseName(theDBname), user(theUser, thePasswd) { }

    ArangoDBConnect():
    serverUrl("http://localhost:8529"), databaseName("_system")  {}

    std::string fullHost() const
    {  return serverUrl+"/_db/"+databaseName; }

    std::string fullURL( const std::string& localpath ) const
    {
       return  fullHost()+localpath;
    }

    bool readonlyDBAccess( ) const
    {
      return( user.access=="ro");
    }

    void getFromSettings(const std::string& group, bool rootdata = false );
};

bool operator!=(const ArangoDBConnect& lhs, const ArangoDBConnect& rhs);


/// Definition of ArangoDB root API Low Level C++ Driver for ArangoDB
class TArangoDBRootClient
{

    /// Connection
    ArangoDBConnect rootData;

    /// ArangoDB database/user data
    std::shared_ptr<arangodb::ArangoDBUsersAPI> pusers;

public:

    /// Constructor
    TArangoDBRootClient( const ArangoDBConnect& rootconnectData )
    {
      resetDBConnection( rootconnectData );
    }

    /// Default Constructor
    TArangoDBRootClient();

    ///  Destructor
    virtual ~TArangoDBRootClient();

    /// Reset connections to ArangoDB server
    void resetDBConnection( const ArangoDBConnect& connectData );
    /// Get ArangoDB connection data
    const jsonio::ArangoDBConnect& dbConnection() const
    {
       return rootData;
    }

    // Database API

    /// Retrieves the list of all existing databases
    std::set<std::string> getDatabaseNames();
    /// Create Data base dbname with list of existing users if no exist
    /// \param users: Login Names of the users to be accessed
    void CreateDatabase( const std::string& dbname, const std::vector<ArangoDBUser>&  users = {} );

    // Users API

    /// Create user
    void CreateUser( const ArangoDBUser& userdata );
    /// Retrieves a map contains the databases names as object keys, and the associated privileges
    /// for the database as values of all databases the current user can access
    std::map<std::string,std::string> getDatabaseNames( const std::string&  user );
    /// Fetches data about all users
    std::set<std::string> getUserNames();

};


} // namespace jsonio

#endif // TDBARANGO_H
