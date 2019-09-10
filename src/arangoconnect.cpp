//  This is JSONIO library+API (https://bitbucket.org/gems4/jsonio)
//
/// \file dbarango.cpp
/// Implementation of class
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
// YAML-CPP (https://github.com/jbeder/yaml-cpp); EJDB (http://ejdb.org).
//

#include <cstring>
#include <iostream>
#include "arangodbusers.h"
#include "jsonio/io_settings.h"

namespace arangocpp {

//ArangoDBConnect TArangoDBClientOne::theConnect( "https://db.thermohub.net",
//           "adminrem",  "Administrator@Remote-ThermoHub-Server",  "hub_test"  );

//ArangoDBConnect TArangoDBClientOne::theConnect( "http://localhost:8529",
//           "root",  "",  "_system"  );


bool operator!=(const ArangoDBUser& lhs, const ArangoDBUser& rhs)
{
    return  lhs.name != rhs.name || lhs.access != rhs.access;
}

bool operator!=(const ArangoDBConnect& lhs, const ArangoDBConnect& rhs)
{
    return lhs.serverUrl != rhs.serverUrl || lhs.user != rhs.user ||
              lhs.databaseName != rhs.databaseName;
}

void ArangoDBConnect::getFromSettings( const std::string& group, bool rootdata )
{
    GroupSettings dbGroup(ioSettings().group(group));

    if( rootdata)
    {
        serverUrl = dbGroup.value( "DB_URL", serverUrl );
        databaseName = dbGroup.value( "DBRootName", databaseName );
        user.name = dbGroup.value( "DBRootUser", user.name );
        user.password = dbGroup.value( "DBRootPassword", user.password  );
        user.access = dbGroup.value( "DBAccess", user.access  );
    }
    else
    {
        serverUrl = dbGroup.value( "DB_URL", serverUrl );
        databaseName = dbGroup.value( "DBName", databaseName );
        user.name = dbGroup.value( "DBUser", user.name );
        user.password = dbGroup.value( "DBUserPassword", user.password  );
        user.access = dbGroup.value( "DBAccess", user.access  );
    }
}

// TArangoDBRootClient ------------------------------------------

/// Default Constructor
TArangoDBRootClient::TArangoDBRootClient()
{
  rootData.getFromSettings(ioSettings().defaultArangoDB(), true );
  resetDBConnection( rootData );
}

TArangoDBRootClient::~TArangoDBRootClient()
{ }

void TArangoDBRootClient::resetDBConnection( const ArangoDBConnect& aconnectData )
{
    rootData = aconnectData;
    pusers.reset( new arangodb::ArangoDBUsersAPI(rootData ) ); /// here must be root data
}

std::set<std::string> TArangoDBRootClient::getDatabaseNames()
{
  return pusers->getDatabaseNames();
}

void TArangoDBRootClient::CreateDatabase( const std::string& dbname, const std::vector<ArangoDBUser>& users )
{
   jsonioErrIf( rootData.readonlyDBAccess(), dbname, "Trying create database into read only mode.");
   pusers->CreateDatabase( dbname, users );
}

void TArangoDBRootClient::CreateUser( const ArangoDBUser& userdata )
{
   jsonioErrIf( rootData.readonlyDBAccess(), userdata.name, "Trying create user into read only mode.");
   return pusers->CreateUser( userdata );
}

std::map<std::string,std::string> TArangoDBRootClient::getDatabaseNames( const std::string&  user )
{
   return pusers->getDatabaseNames( user );
}

std::set<std::string> TArangoDBRootClient::getUserNames()
{
   return pusers->getUserNames();
}

} // namespace arangocpp
