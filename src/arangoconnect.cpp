#include <fstream>
#include <sstream>
#include "arango-cpp/arangodbusers.h"
#include "arango-cpp/arangoexception.h"
#include "arangodetail.h"
#include "arangorequests.h"

namespace arangocpp {

// Full list of edges used in connection query.
std::vector<std::string> ArangoDBConnection::full_list_of_edges{};
// Use content type Velocypack on sending requests (fu_content_type_vpack)
bool ArangoDBConnection::use_velocypack_put = true;
// Use content type Velocypack on getting results
bool ArangoDBConnection::use_velocypack_get = true;


const char* ArangoDBConnection::local_server_endpoint = "http://localhost:8529";
const char* ArangoDBConnection::local_server_username  = "root";
const char* ArangoDBConnection::local_server_password  = "";
const char* ArangoDBConnection::local_server_database  = "_system";
const char* ArangoDBConnection::local_root_username  = "root";
const char* ArangoDBConnection::local_root_password  = "";
const char* ArangoDBConnection::local_root_database  = "_system";
const char* ArangoDBConnection::remote_server_endpoint = "https://db.thermohub.net";
const char* ArangoDBConnection::remote_server_username  = "__put_here_the_user_name__";
const char* ArangoDBConnection::remote_server_password  = "__put_here_the_remote_password__";
const char* ArangoDBConnection::remote_server_database  = "_system";


bool operator==(const ArangoDBUser& lhs, const ArangoDBUser& rhs)
{
    return  lhs.name == rhs.name && lhs.access == rhs.access;
}

bool operator!=(const ArangoDBUser& lhs, const ArangoDBUser& rhs)
{
    return !(lhs==rhs);
}

bool operator==(const ArangoDBConnection& lhs, const ArangoDBConnection& rhs)
{
    return lhs.serverUrl == rhs.serverUrl && lhs.user == rhs.user &&
            lhs.databaseName == rhs.databaseName;
}
bool operator!=(const ArangoDBConnection& lhs, const ArangoDBConnection& rhs)
{
    return !(lhs==rhs);
}


// Get settings data from json string
ArangoDBConnection connectFromSettings( const std::string& jsonstr, bool rootdata )
{
    ArangoDBConnection connect_data;

    try{
        auto data = ::arangodb::velocypack::Parser::fromJson(jsonstr);

        auto slice = data->slice();
        auto slicedb = slice.get("arangodb");
        if( slicedb.isObject() ) {
            auto instance = slicedb.get( "UseArangoDBInstance" ).copyString();
            ArangoDBConnection::use_velocypack_put = slicedb.get( "UseVelocypackPut" ).getBool();
            ArangoDBConnection::use_velocypack_get = slicedb.get( "UseVelocypackGet" ).getBool();
            slicedb = slicedb.get(instance);

            if( slicedb.isObject() ) {
                if( rootdata ) {
                    connect_data.serverUrl = slicedb.get( "DB_URL" ).copyString();
                    connect_data.databaseName = slicedb.get( "DBRootName" ).copyString();
                    connect_data.user.name = slicedb.get( "DBRootUser").copyString();
                    connect_data.user.password = slicedb.get( "DBRootPassword" ).copyString();
                    connect_data.user.access = "rw";
                }
                else {
                    connect_data.serverUrl = slicedb.get( "DB_URL" ).copyString();
                    connect_data.databaseName = slicedb.get( "DBName" ).copyString();
                    connect_data.user.name = slicedb.get( "DBUser" ).copyString();
                    connect_data.user.password = slicedb.get( "DBUserPassword" ).copyString();
                    connect_data.user.access = slicedb.get( "DBAccess" ).copyString();
                }
            }
        }
    }
    catch (::arangodb::velocypack::Exception& error )  {
        arango_logger->warn("Read configuration data error: {}", jsonstr);
        ARANGO_THROW( "ArangoDBRootClient", 4, std::string("Read configuration file error: ")+error.what());
    }
    return connect_data;
}

ArangoDBConnection connectFromConfig( const std::string& cfgfile )
{
    auto jsonstr = detail::read_all_file( cfgfile );
    return connectFromSettings( jsonstr, false );
}

ArangoDBConnection rootClientFromConfig( const std::string& cfgfile )
{
    auto jsonstr = detail::read_all_file( cfgfile );
    return connectFromSettings( jsonstr, true );
}

// ArangoDBRootClient ------------------------------------------

void ArangoDBRootClient::resetDBConnection( const ArangoDBConnection& aconnectData )
{
    rootData = aconnectData;
    pusers.reset( new ArangoDBUsersAPI(rootData) ); /// here must be root data
    /// posible check exist connection
}

bool ArangoDBRootClient::existDatabase(const std::string &dbname )
{
   return pusers->existDatabase( dbname );
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
