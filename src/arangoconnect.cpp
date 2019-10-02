#include <fstream>
#include <sstream>
#include "jsonarango/arangodbusers.h"
#include "jsonarango/arangoexception.h"
#include "arangodetail.h"

namespace arangocpp {

// Full list of edges used in connection query.
std::vector<std::string> ArangoDBConnect::full_list_of_edges{};
// Use content type Velocypack on sending requests (fu_content_type_vpack)
bool ArangoDBConnect::use_velocypack_put = true;
// Use content type Velocypack on getting results
bool ArangoDBConnect::use_velocypack_get = true;

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

/// Get settings data from json string
ArangoDBConnect connectFromSettings( const std::string& jsonstr, bool rootdata )
{
    ArangoDBConnect connect_data;

    try{
        auto data = ::arangodb::velocypack::Parser::fromJson(jsonstr);

        auto slice = data->slice();
        auto slicedb = slice.get("arangodb");
        if( slicedb.isObject() )
        {
            auto instance = slicedb.get( "UseArangoDBInstance" ).copyString();
            ArangoDBConnect::use_velocypack_put = slicedb.get( "UseVelocypackPut" ).getBool();
            ArangoDBConnect::use_velocypack_get = slicedb.get( "UseVelocypackGet" ).getBool();
            slicedb = slicedb.get(instance);

            if( slicedb.isObject() )
            {
                if( rootdata)
                {
                    connect_data.serverUrl = slicedb.get( "DB_URL" ).copyString();
                    connect_data.databaseName = slicedb.get( "DBRootName" ).copyString();
                    connect_data.user.name = slicedb.get( "DBRootUser").copyString();
                    connect_data.user.password = slicedb.get( "DBRootPassword" ).copyString();
                    connect_data.user.access = "rw";
                }
                else
                {
                    connect_data.serverUrl = slicedb.get( "DB_URL" ).copyString();
                    connect_data.databaseName = slicedb.get( "DBName" ).copyString();
                    connect_data.user.name = slicedb.get( "DBUser" ).copyString();
                    connect_data.user.password = slicedb.get( "DBUserPassword" ).copyString();
                    connect_data.user.access = slicedb.get( "DBAccess" ).copyString();
                }
            }
        }
    }
    catch (::arangodb::velocypack::Exception& error )
    {
        JSONIO_LOG << "Read configuration file error: " << error.what() << std::endl;
        ARANGO_THROW( "ArangoDBRootClient", 4, std::string("Read configuration file error: ")+error.what());
    }
    return connect_data;
}


ArangoDBConnect connectFromConfig( const std::string& cfgfile )
{
    auto jsonstr = detail::read_all_file( cfgfile );
    return connectFromSettings( jsonstr, false );
}

ArangoDBConnect rootClientFromConfig( const std::string& cfgfile )
{
    auto jsonstr = detail::read_all_file( cfgfile );
    return connectFromSettings( jsonstr, true );
}


// ArangoDBRootClient ------------------------------------------


void ArangoDBRootClient::resetDBConnection( const ArangoDBConnect& aconnectData )
{
    rootData = aconnectData;
    pusers.reset( new ArangoDBUsersAPI(rootData) ); /// here must be root data
}

std::set<std::string> ArangoDBRootClient::databaseNames()
{
    return pusers->databaseNames();
}

void ArangoDBRootClient::createDatabase( const std::string& dbname, const std::vector<ArangoDBUser>& users )
{
    pusers->createDatabase( dbname, users );
}

void ArangoDBRootClient::createUser( const ArangoDBUser& userdata )
{
    return pusers->createUser( userdata );
}

std::map<std::string,std::string> ArangoDBRootClient::databaseNames( const std::string&  user )
{
    return pusers->databaseNames( user );
}

std::set<std::string> ArangoDBRootClient::userNames()
{
    return pusers->userNames();
}

namespace detail {

// Read whole ASCII file into string.
std::string read_all_file( const std::string& file_path )
{
    std::ifstream t(file_path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

}

} // namespace arangocpp
