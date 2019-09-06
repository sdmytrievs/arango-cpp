#include "arangodbusers.h"
#include "arangocurl.h"

namespace jsonio { namespace arangodb {

// Send a request to the server and wait into a response it received.
std::unique_ptr<arangodb::HttpMessage> ArangoDBUsersAPI::sendRequest(std::unique_ptr<arangodb::HttpMessage> rq )
{
   //try{
        DEBUG_OUTPUT( "request", rq );
        auto url = _connect_data.serverUrl+rq->header.path;
        arangodb::RequestCurlObject mco( url, _connect_data.user.name, _connect_data.user.password, std::move(rq) );
        auto result = mco.getResponse();
        DEBUG_OUTPUT( "result", result );
        if( !result->isContentTypeVPack() )
            jsonioErr("DBArango002: ", "Illegal content type" );
        return result;
   //}catch (arangodb::ErrorCondition& error )
   // {
   //     cout << "Response from server: " << arangodb::to_std::string(error) << endl;
   //     jsonioErr("DBArango001: ", "error response from server", arangodb::to_std::string(error));
   // }
}


std::set<std::string> ArangoDBUsersAPI::getDatabaseNames()
 {
   std::set<std::string> dbnames;
   auto request = createRequest(arangodb::RestVerb::Get, std::string("/_api/database")  );
   auto result =  sendRequest(std::move(request));

   if( result->statusCode() == arangodb::StatusOK )
   {
       auto slice = result->slices().front();
       auto collst = slice.get("result");
       for( uint ii=0; ii<collst.length(); ii++ )
         dbnames.insert( collst[ii].copyString());
   }
   return dbnames;
 }


std::map<std::string,std::string> ArangoDBUsersAPI::getDatabaseNames(const std::string&  user)
 {
   std::map<std::string,std::string> dbnames;
   std::string qpath  = std::string("/_api/user/")+user+"/database/";
   auto request = createRequest(arangodb::RestVerb::Get, qpath  );
   auto result =  sendRequest(std::move(request));

   if( result->statusCode() == arangodb::StatusOK )
   {
       auto slice = result->slices().front();
       auto collst = slice.get("result");
       for( uint ii=0; ii<collst.length(); ii++ )
          dbnames[collst.keyAt(ii).copyString()] = collst.valueAt(ii).copyString();
   }

   return dbnames;
 }

// Information of the database
bool ArangoDBUsersAPI::ExistDatabase( const std::string& dbname )
{
    std::string qpath  = std::string("/_db/") + dbname + std::string("/_api/database/current") ;
    auto request = createRequest(arangodb::RestVerb::Get, qpath );
    auto result = sendRequest(std::move(request));
    return result->statusCode() != arangodb::StatusNotFound;
}

void ArangoDBUsersAPI::CreateDatabase( const std::string& dbname, const std::vector<ArangoDBUser>&  users )
{
    if( ExistDatabase( dbname ) )
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
                  auto data = ::arangodb::velocypack::Parser::fromJson( user.extra );
                  builder.add("extra" , data->slice() );
               }
              builder.close();
           }
           builder.close();
       }
       builder.close();

       auto request = createRequest(arangodb::RestVerb::Post, std::string("/_api/database"));
       request->addVPack(builder.slice());
       auto result = sendRequest(std::move(request));

       if( result->statusCode() != arangodb::StatusCreated )
       {
           auto errmsg = result->slices().front().get("errorMessage").copyString();
           jsonioErr( "DBArango010: ", "Error when create database: "+errmsg );
       }
    }
    catch (::arangodb::velocypack::Exception& error )
        {
            std::cout << "Velocypack error: " << error.what() << std::endl;
            jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
        }
}

void ArangoDBUsersAPI::RemoveDatabase( const std::string& dbname )
{
    if( !ExistDatabase( dbname ) )
      return;

    std::string qpath  = std::string("/_api/database/")+dbname;
    auto request = createRequest(arangodb::RestVerb::Delete, qpath );
    auto result = sendRequest(std::move(request));

    if( result->statusCode() != arangodb::StatusOK )
    {
        auto errmsg = result->slices().front().get("errorMessage").copyString();
        jsonioErr( "DBArango011: ", "Error when drop database: "+errmsg );
    }
}

void ArangoDBUsersAPI::CreateUser( const ArangoDBUser& userdata )
{
    try{
       ::arangodb::velocypack::Builder builder;
       builder.openObject();
       builder.add("user" , ::arangodb::velocypack::Value(userdata.name) );
       builder.add("passwd" , ::arangodb::velocypack::Value(userdata.password) );
       builder.add("active" , ::arangodb::velocypack::Value(userdata.active) );
       if( !userdata.extra.empty() )
       {
           auto data = ::arangodb::velocypack::Parser::fromJson(userdata.extra);
           builder.add("extra" , data->slice() );
       }
       builder.close();

       auto request = createRequest(arangodb::RestVerb::Post, std::string("/_api/user"));
       request->addVPack(builder.slice());
       auto result = sendRequest(std::move(request));

       if( result->statusCode() != arangodb::StatusCreated )
       {
           auto errmsg = result->slices().front().get("errorMessage").copyString();
           jsonioErr( "DBArango012: ", "Error when create user: "+errmsg );
       }
    }
    catch (::arangodb::velocypack::Exception& error )
        {
            std::cout << "Velocypack error: " << error.what() << std::endl;
            jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
        }
}

void ArangoDBUsersAPI::UpdateUser( const ArangoDBUser& userdata )
{
    try{
       ::arangodb::velocypack::Builder builder;
       builder.openObject();
       builder.add("passwd" , ::arangodb::velocypack::Value(userdata.password) );
       builder.add("active" , ::arangodb::velocypack::Value(userdata.active) );
       if( !userdata.extra.empty() )
       {
           auto data = ::arangodb::velocypack::Parser::fromJson(userdata.extra);
           builder.add("extra" , data->slice() );
       }
       builder.close();

       auto request = createRequest(arangodb::RestVerb::Patch, std::string("/_api/user/")+userdata.name);
       request->addVPack(builder.slice());
       auto result = sendRequest(std::move(request));

       if( result->statusCode() != arangodb::StatusOK )
       {
           auto errmsg = result->slices().front().get("errorMessage").copyString();
           jsonioErr( "DBArango013: ", "Error when update user data: "+errmsg );
       }
    }
    catch (::arangodb::velocypack::Exception& error )
        {
            std::cout << "Velocypack error: " << error.what() << std::endl;
            jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
        }
}

void ArangoDBUsersAPI::RemoveUser( const std::string& username )
{
    std::string qpath  = std::string("/_api/user/")+username;
    auto request = createRequest(arangodb::RestVerb::Delete, qpath );
    auto result = sendRequest(std::move(request));

    if( result->statusCode() != arangodb::StatusAccepted )
    {
        auto errmsg = result->slices().front().get("errorMessage").copyString();
        jsonioErr( "DBArango014: ", "Error when drop user: "+errmsg );
    }
}

void ArangoDBUsersAPI::GrantUserToDataBase(const std::string& dbname, const std::string& username, const std::string& grant )
{
    ::arangodb::velocypack::Builder builder;
    builder.openObject();
    builder.add("grant", ::arangodb::velocypack::Value(grant) );
    builder.close();

    std::string qpath  = std::string("/_api/user/")+ username + "/database/"+ dbname;
    auto request = createRequest(arangodb::RestVerb::Put, qpath );
    request->addVPack(builder.slice());
    auto result = sendRequest(std::move(request));

    if( result->statusCode() != arangodb::StatusOK )
    {
        auto errmsg = result->slices().front().get("errorMessage").copyString();
        jsonioErr( "DBArango015: ", "Error when grant user: "+errmsg );
    }
}


std::set<std::string> ArangoDBUsersAPI::getUserNames()
 {
   std::set<std::string> usernames;
   auto request = createRequest(arangodb::RestVerb::Get, std::string("/_api/user/")  );
   auto result =  sendRequest(std::move(request));

   if( result->statusCode() == arangodb::StatusOK )
   {
       auto slice = result->slices().front();
       auto collst = slice.get("result");
       for( uint ii=0; ii<collst.length(); ii++ )
         usernames.insert( collst[ii].get("user").copyString());
   }
   return usernames;
 }


}  }
