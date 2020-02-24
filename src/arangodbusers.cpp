#include "jsonarango/arangodbusers.h"
#include "jsonarango/arangoexception.h"
#include "arangocurl.h"


namespace arangocpp {

// Send a request to the server and wait into a response it received.
std::unique_ptr<HttpMessage> ArangoDBUsersAPI::sendREQUEST(std::unique_ptr<HttpMessage> rq )
{
    //try{
    DEBUG_OUTPUT( "request", rq )
            auto url = connect_data.serverUrl+rq->header.path;
    //RequestCurlObject mco( url, connect_data.user.name, connect_data.user.password, std::move(rq) );
    //auto result = mco.getResponse();
    curl_object->sendRequest(url, connect_data.user.name, connect_data.user.password, std::move(rq));
    auto result = curl_object->getResponse();
    DEBUG_OUTPUT( "result", result )
            if( !result->isContentTypeVPack() )
            ARANGO_THROW( "ArangoDBUsersAPI", 42, "Illegal content type" );
    return result;
    //}catch (ErrorCondition& error )
    // {
    //     cout << "Response from server: " << to_std::string(error) << endl;
    //     jsonioErr("DBArango001: ", "error response from server", to_std::string(error));
    // }
}


std::set<std::string> ArangoDBUsersAPI::databaseNames()
{
    std::set<std::string> dbnames;
    auto request = createREQUEST(RestVerb::Get, std::string("/_api/database")  );
    auto result =  sendREQUEST(std::move(request));

    if( result->statusCode() == StatusOK )
    {
        auto slice = result->slices().front();
        auto collst = slice.get("result");
        for( size_t ii=0; ii<collst.length(); ii++ )
            dbnames.insert( collst[ii].copyString());
    }
    return dbnames;
}


std::map<std::string,std::string> ArangoDBUsersAPI::databaseNames(const std::string&  user)
{
    std::map<std::string,std::string> dbnames;
    std::string qpath  = std::string("/_api/user/")+user+"/database/";
    auto request = createREQUEST(RestVerb::Get, qpath  );
    auto result =  sendREQUEST(std::move(request));

    if( result->statusCode() == StatusOK )
    {
        auto slice = result->slices().front();
        auto collst = slice.get("result");
        for( size_t ii=0; ii<collst.length(); ii++ )
            dbnames[collst.keyAt(ii).copyString()] = collst.valueAt(ii).copyString();
    }

    return dbnames;
}

// Information of the database
bool ArangoDBUsersAPI::existDatabase( const std::string& dbname )
{
    std::string qpath  = std::string("/_db/") + dbname + std::string("/_api/database/current") ;
    auto request = createREQUEST(RestVerb::Get, qpath );
    auto result = sendREQUEST(std::move(request));
    return result->statusCode() != StatusNotFound;
}

void ArangoDBUsersAPI::createDatabase( const std::string& dbname, const std::vector<ArangoDBUser>&  users )
{
    if( existDatabase( dbname ) )
        return;

    try{
        ::arangodb::velocypack::Builder builder;
        builder.openObject();
        builder.add("name" , ::arangodb::velocypack::Value(dbname) );
        if( !users.empty() )
        {
            builder.add("users" ,  ::arangodb::velocypack::Value(::arangodb::velocypack::ValueType::Array) );
            for( auto user: users )
            {
                builder.add( ::arangodb::velocypack::Value(::arangodb::velocypack::ValueType::Object ) );
                builder.add("username" , ::arangodb::velocypack::Value(user.name) );
                builder.add("passwd" , ::arangodb::velocypack::Value(user.password) );
                builder.add("active" , ::arangodb::velocypack::Value(user.active) );
                if( !user.extra.empty() )
                {
                    auto data = ::arangodb::velocypack::Parser::fromJson( user.extra, &parse_options );
                    builder.add("extra" , data->slice() );
                }
                builder.close();
            }
            builder.close();
        }
        builder.close();

        auto request = createREQUEST(RestVerb::Post, std::string("/_api/database"));
        request->addVPack(builder.slice());
        auto result = sendREQUEST(std::move(request));

        if( result->statusCode() != StatusCreated )
        {
            auto errmsg = result->slices().front().get("errorMessage").copyString();
            ARANGO_THROW( "ArangoDBUsersAPI", 43, std::string("Error when create database: ")+errmsg );
        }
    }
    catch (::arangodb::velocypack::Exception& error )
    {
        JSONIO_LOG << "Velocypack error: " << error.what() << std::endl;
        ARANGO_THROW( "ArangoDBUsersAPI", 41, std::string("Velocypack error: ")+error.what());
    }
}

void ArangoDBUsersAPI::removeDatabase( const std::string& dbname )
{
    if( !existDatabase( dbname ) )
        return;

    std::string qpath  = std::string("/_api/database/")+dbname;
    auto request = createREQUEST(RestVerb::Delete, qpath );
    auto result = sendREQUEST(std::move(request));

    if( result->statusCode() != StatusOK )
    {
        auto errmsg = result->slices().front().get("errorMessage").copyString();
        ARANGO_THROW( "ArangoDBUsersAPI", 44, std::string("Error when drop database: ")+errmsg );
    }
}

void ArangoDBUsersAPI::createUser( const ArangoDBUser& userdata )
{
    try{
        ::arangodb::velocypack::Builder builder;
        builder.openObject();
        builder.add("user" , ::arangodb::velocypack::Value(userdata.name) );
        builder.add("passwd" , ::arangodb::velocypack::Value(userdata.password) );
        builder.add("active" , ::arangodb::velocypack::Value(userdata.active) );
        if( !userdata.extra.empty() )
        {
            auto data = ::arangodb::velocypack::Parser::fromJson(userdata.extra, &parse_options);
            builder.add("extra" , data->slice() );
        }
        builder.close();

        auto request = createREQUEST(RestVerb::Post, std::string("/_api/user"));
        request->addVPack(builder.slice());
        auto result = sendREQUEST(std::move(request));

        if( result->statusCode() != StatusCreated )
        {
            auto errmsg = result->slices().front().get("errorMessage").copyString();
            ARANGO_THROW( "ArangoDBUsersAPI", 45, std::string("Error when create user: ")+errmsg );
        }
    }
    catch (::arangodb::velocypack::Exception& error )
    {
        JSONIO_LOG << "Velocypack error: " << error.what() << std::endl;
        ARANGO_THROW( "ArangoDBUsersAPI", 41, std::string("Velocypack error: ")+error.what());
    }
}

void ArangoDBUsersAPI::updateUser( const ArangoDBUser& userdata )
{
    try{
        ::arangodb::velocypack::Builder builder;
        builder.openObject();
        builder.add("passwd" , ::arangodb::velocypack::Value(userdata.password) );
        builder.add("active" , ::arangodb::velocypack::Value(userdata.active) );
        if( !userdata.extra.empty() )
        {
            auto data = ::arangodb::velocypack::Parser::fromJson(userdata.extra, &parse_options);
            builder.add("extra" , data->slice() );
        }
        builder.close();

        auto request = createREQUEST(RestVerb::Patch, std::string("/_api/user/")+userdata.name);
        request->addVPack(builder.slice());
        auto result = sendREQUEST(std::move(request));

        if( result->statusCode() != StatusOK )
        {
            auto errmsg = result->slices().front().get("errorMessage").copyString();
            ARANGO_THROW( "ArangoDBUsersAPI", 46, std::string("Error when update user data: ")+errmsg );
        }
    }
    catch (::arangodb::velocypack::Exception& error )
    {
        JSONIO_LOG << "Velocypack error: " << error.what() << std::endl;
        ARANGO_THROW( "ArangoDBUsersAPI", 41, std::string("Velocypack error: ")+error.what());
    }
}

void ArangoDBUsersAPI::removeUser( const std::string& username )
{
    std::string qpath  = std::string("/_api/user/")+username;
    auto request = createREQUEST(RestVerb::Delete, qpath );
    auto result = sendREQUEST(std::move(request));

    if( result->statusCode() != StatusAccepted )
    {
        auto errmsg = result->slices().front().get("errorMessage").copyString();
        ARANGO_THROW( "ArangoDBUsersAPI", 47, std::string("Error when drop user: ")+errmsg );
    }
}

void ArangoDBUsersAPI::grantUserToDataBase(const std::string& dbname, const std::string& username, const std::string& grant )
{
    ::arangodb::velocypack::Builder builder;
    builder.openObject();
    builder.add("grant", ::arangodb::velocypack::Value(grant) );
    builder.close();

    std::string qpath  = std::string("/_api/user/")+ username + "/database/"+ dbname;
    auto request = createREQUEST(RestVerb::Put, qpath );
    request->addVPack(builder.slice());
    auto result = sendREQUEST(std::move(request));

    if( result->statusCode() != StatusOK )
    {
        auto errmsg = result->slices().front().get("errorMessage").copyString();
        ARANGO_THROW( "ArangoDBUsersAPI", 48, std::string("Error when grant user: ")+errmsg );
    }
}


std::set<std::string> ArangoDBUsersAPI::userNames()
{
    std::set<std::string> usernames;
    auto request = createREQUEST(RestVerb::Get, std::string("/_api/user/")  );
    auto result =  sendREQUEST(std::move(request));

    if( result->statusCode() == StatusOK )
    {
        auto slice = result->slices().front();
        auto collst = slice.get("result");
        for( size_t ii=0; ii<collst.length(); ii++ )
            usernames.insert( collst[ii].get("user").copyString());
    }
    return usernames;
}


} // namespace arangocpp
