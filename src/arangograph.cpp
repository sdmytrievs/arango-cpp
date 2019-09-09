#include <iostream>
#include "arangograph.h"

namespace jsonio { namespace arangodb {

ArangoDBGraphAPI::~ArangoDBGraphAPI()
{ }

// Test exist collection
bool ArangoDBGraphAPI::ExistGraph(const std::string& graphname )
{
    std::string qpath  = std::string("/_api/gharial/")+graphname;
    auto request = createRequest(arangodb::RestVerb::Get, qpath );
    request->header.meta.erase("content-type");
    request->header.meta.erase("accept");
    auto result = sendRequest(std::move(request));

    return result->statusCode() != arangodb::StatusNotFound;
}

// Create graph
void ArangoDBGraphAPI::CreateGraph(const std::string& graphname, const std::string& edgeDefinitions)
{
    // test exist
  if( ExistGraph( graphname ) )
      return;

 try {
    auto data = ::arangodb::velocypack::Parser::fromJson(edgeDefinitions);

    ::arangodb::velocypack::Builder builder;
    builder.openObject();
    builder.add("name" , ::arangodb::velocypack::Value(std::string(graphname)));
    builder.add("edgeDefinitions" , data->slice() );
    builder.close();

    auto request1 = createRequest(arangodb::RestVerb::Post, std::string("/_api/gharial") );
    request1->addVPack(builder.slice());
    request1->header.meta.erase("accept");
    auto result1 = sendRequest(std::move(request1));

    if( result1->statusCode() == arangodb::StatusConflict )
        jsonioErr( "DBArango013: ", "This can occur either if a graph with this name is already stored,"
                   "or if there is one edge definition with a the same edge collection but a different "
                   "signature used in any other graph." );
 } catch (::arangodb::velocypack::Exception& error )
     {
         std::cout << "Velocypack error: " << error.what() << std::endl;
         jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
     }
}

std::set<std::string> ArangoDBGraphAPI::getGraphCollectionNames(
        const std::string& graphname, const std::string& colltype )
{
  std::set<std::string> collnames;
  std::string rqstr = "/_api/gharial/"+graphname+ "/"+colltype;
  auto request = createRequest(arangodb::RestVerb::Get, rqstr  );
  request->header.meta.erase("content-type");
  //request->header.meta.erase("accept");
  auto result =  sendRequest(std::move(request));

  if( result->statusCode() == arangodb::StatusOK )
  {
      auto slice = result->slices().front();
      auto collst = slice.get("collections");

      auto numb = collst.length();
      for( uint ii=0; ii<numb; ii++ )
          collnames.insert(collst[ii].copyString());
  }

  return collnames;
}

void ArangoDBGraphAPI::AddVertexGraph(const std::string& graphname, const std::string& collname )
{
    ::arangodb::velocypack::Builder builder;
    builder.openObject();
    builder.add("collection" , ::arangodb::velocypack::Value(std::string(collname)));
    builder.close();

    std::string rqstr = "/_api/gharial/"+graphname+ "/vertex";
    auto request1 = createRequest(arangodb::RestVerb::Post, rqstr );
    request1->addVPack(builder.slice());
    request1->header.meta.erase("accept");
    auto result1 = sendRequest(std::move(request1));

    if( result1->statusCode() >=  arangodb::StatusBadRequest )
        jsonioErr( "DBArango014: ", "Error when add collection" );
}

void ArangoDBGraphAPI::AddEdgeGraph(const std::string& graphname, const std::string& edgeDefinition)
{
 try {
    auto data = ::arangodb::velocypack::Parser::fromJson(edgeDefinition);

    std::string rqstr = "/_api/gharial/"+graphname+ "/edge";
    auto request1 = createRequest(arangodb::RestVerb::Post, rqstr);
    request1->addVPack(data->slice());
    request1->header.meta.erase("accept");
    auto result1 = sendRequest(std::move(request1));

    if( result1->statusCode() >=  arangodb::StatusBadRequest )
        jsonioErr( "DBArango015: ", "The defininition could not be added,"
                   "the edge collection is used in an other graph with a different signature" );
 } catch (::arangodb::velocypack::Exception& error )
     {
         std::cout << "Velocypack error: " << error.what() << std::endl;
         jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
     }
}


//----------------------------------------------------------------------------

// Retrive one record from the collection
bool ArangoDBGraphAPI::ReadRecord( const std::string& graphname, const std::string& colltype,
   const std::string& collname, const std::string& key, std::string& jsonrec )
{
    std::string rid = getId( collname, key );

    std::string rqstr = "/_api/gharial/"+graphname+ "/"+colltype+"/"+rid;
    auto request = createRequest(arangodb::RestVerb::Get, rqstr  );
    request->header.meta.erase("content-type");
    request->header.meta.erase("accept");
    auto result =  sendRequest(std::move(request));
    auto slice = result->slices().front();

    if( result->statusCode() != arangodb::StatusOK )
    {
       auto errmsg = slice.get("errorMessage").copyString();
       JSONIO_LOG << "Error :" << errmsg << std::endl;
       jsonioErr( "DBArango010: ", std::string("Error when try load record: ") + errmsg);
    }
     else
       {
         jsonrec =  slice.get(colltype).toJson(&_dumpoptions);
         JSONIO_LOG << "readRecord :" << jsonrec << std::endl;
         return true;
        }
    //return false;
}


std::string ArangoDBGraphAPI::CreateRecord( const std::string& graphname, const std::string& colltype,
      const std::string& collname, const std::string& jsonrec )
{
    std::string newId = "";

    try{
       auto data = ::arangodb::velocypack::Parser::fromJson(jsonrec, &_parseoptions);

       std::string rqstr = "/_api/gharial/"+graphname+ "/"+colltype+"/"+collname;
       auto request = createRequest(arangodb::RestVerb::Post, rqstr  );
       request->addVPack(data->slice());
       request->header.meta.erase("accept");
       auto result =  sendRequest(std::move(request));
       auto slice1 = result->slices().front();

       if( result->statusCode() >=  arangodb::StatusBadRequest )
       {
         auto errmsg = slice1.get("errorMessage").copyString();
         JSONIO_LOG << "Error :" << errmsg << std::endl;
         jsonioErr( "DBArango010: ", std::string("Error when try create record: ") + errmsg);
      }
      else
       {
         newId= slice1.get(colltype).get("_id").copyString();
         JSONIO_LOG << "createRecord :" << newId << std::endl;
       }
   } catch (::arangodb::velocypack::Exception& error )
    {
        std::cout << "Velocypack error: " << error.what() << std::endl;
        jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
    }

   return newId;
}

std::string ArangoDBGraphAPI::UpdateRecord( const std::string& graphname, const std::string& colltype,
        const std::string& collname, const std::string& key, const std::string& jsonrec )
{
    std::string newId = "";
    std::string rid = getId( collname, key );

    try{
        auto data = ::arangodb::velocypack::Parser::fromJson(jsonrec);

        std::string rqstr = "/_api/gharial/"+graphname+ "/"+colltype+"/"+rid;;
        auto request = createRequest(arangodb::RestVerb::Put, rqstr  );
        request->addVPack(data->slice());
        request->header.meta.erase("accept");
        auto result =  sendRequest(std::move(request));
        auto slice1 = result->slices().front();

        if( result->statusCode() >=  arangodb::StatusBadRequest )
        {
           auto errmsg = slice1.get("errorMessage").copyString();
           JSONIO_LOG << "Error :" << errmsg << std::endl;
           jsonioErr( "DBArango010: ", std::string("Error when try save record: ") + errmsg);
        }
        else
        {
           newId=slice1.get(colltype).get("_id").copyString();
           JSONIO_LOG << "updateRecord :" << newId << std::endl;
        }
    } catch (::arangodb::velocypack::Exception& error )
     {
         std::cout << "Velocypack error: " << error.what() << std::endl;
         jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
     }
    return newId;
}

// delete one record from the collection
bool ArangoDBGraphAPI::DeleteRecord( const std::string& graphname, const std::string& colltype,
        const std::string& collname, const std::string& key )
{
    std::string rid = getId( collname, key );

    std::string rqstr = "/_api/gharial/"+graphname+ "/"+colltype+"/"+rid;
    auto request = createRequest(arangodb::RestVerb::Delete, rqstr  );
    request->header.meta.erase("content-type");
    //request->header.meta.erase("accept");
    auto result =  sendRequest(std::move(request));

    if( result->statusCode() >=  arangodb::StatusBadRequest )
    {
       auto slice = result->slices().front();
       auto errmsg = slice.get("errorMessage").copyString();
       JSONIO_LOG << "Error :" << errmsg << std::endl;
       jsonioErr( "DBArango010: ", std::string("Error when try remove record: ") + errmsg);
       //return false;
    }
    return true;
}


}  }
