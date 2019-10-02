#include "jsonarango/arangograph.h"
#include "jsonarango/arangoexception.h"

namespace arangocpp {

ArangoDBGraphAPI::~ArangoDBGraphAPI()
{ }

// Test exist collection
bool ArangoDBGraphAPI::existGraph(const std::string& graphname )
{
    std::string qpath  = std::string("/_api/gharial/")+graphname;
    auto request = createREQUEST(RestVerb::Get, qpath );
    request->header.meta.erase("content-type");
    request->header.meta.erase("accept");
    auto result = sendREQUEST(std::move(request));

    return result->statusCode() != StatusNotFound;
}


// Lists all graph names stored in this database.
std::set<std::string> ArangoDBGraphAPI::graphNames()
{
    std::set<std::string> graph_names;
    std::string rqstr = "/_api/gharial";
    auto request = createREQUEST(RestVerb::Get, rqstr );
    auto result =  sendREQUEST(std::move(request));

    if( result->statusCode() == StatusOK )
    {
        auto slice = result->slices().front();
        auto grlst = slice.get("graphs");
        for( size_t ii=0; ii<grlst.length(); ii++ )
            graph_names.insert( grlst[ii].get("_key").copyString());
    }
    return graph_names;
}


// Create graph
void ArangoDBGraphAPI::createGraph(const std::string& graphname, const std::string& edgeDefinitions)
{
    // test exist
    if( existGraph( graphname ) )
        return;

    try {
        auto data = ::arangodb::velocypack::Parser::fromJson(edgeDefinitions, &parse_options);

        ::arangodb::velocypack::Builder builder;
        builder.openObject();
        builder.add("name" , ::arangodb::velocypack::Value(std::string(graphname)));
        builder.add("edgeDefinitions" , data->slice() );
        builder.close();

        auto request1 = createREQUEST(RestVerb::Post, std::string("/_api/gharial") );
        request1->addVPack(builder.slice());
        request1->header.meta.erase("accept");
        auto result1 = sendREQUEST(std::move(request1));

        if( result1->statusCode() > StatusAccepted )
            ARANGO_THROW( "ArangoDBGraphAPI", 32, "This can occur either if a graph with this name is already stored,"
                                                  "or if there is one edge definition with a the same edge collection but a different "
                                                  "signature used in any other graph." );
    } catch (::arangodb::velocypack::Exception& error )
    {
        JSONIO_LOG << "Velocypack error: " << error.what() << std::endl;
        ARANGO_THROW( "ArangoDBGraphAPI", 31,  std::string("Velocypack error: ")+error.what());
    }
}


void ArangoDBGraphAPI::removeGraph( const std::string& graphname )
{
    if( !existGraph( graphname ) )
        return;

    std::string rqstr = "/_api/gharial/"+graphname;
    auto request = createREQUEST(RestVerb::Delete, rqstr );
    auto result = sendREQUEST(std::move(request));

    if( result->statusCode() >  StatusAccepted )
    {
        auto errmsg = result->slices().front().get("errorMessage").copyString();
        ARANGO_THROW( "ArangoDBGraphAPI", 39, std::string("Error when drop graph: ")+errmsg );
    }
}

std::set<std::string> ArangoDBGraphAPI::graphCollectionNames(
        const std::string& graphname, const std::string& colltype )
{
    std::set<std::string> collnames;
    std::string rqstr = "/_api/gharial/"+graphname+ "/"+colltype;
    auto request = createREQUEST(RestVerb::Get, rqstr  );
    request->header.meta.erase("content-type");
    //request->header.meta.erase("accept");
    auto result =  sendREQUEST(std::move(request));

    if( result->statusCode() == StatusOK )
    {
        auto slice = result->slices().front();
        auto collst = slice.get("collections");

        auto numb = collst.length();
        for( uint ii=0; ii<numb; ii++ )
            collnames.insert(collst[ii].copyString());
    }

    return collnames;
}

void ArangoDBGraphAPI::addVertexGraph(const std::string& graphname, const std::string& collname )
{
    ::arangodb::velocypack::Builder builder;
    builder.openObject();
    builder.add("collection" , ::arangodb::velocypack::Value(std::string(collname)));
    builder.close();

    std::string rqstr = "/_api/gharial/"+graphname+ "/vertex";
    auto request1 = createREQUEST(RestVerb::Post, rqstr );
    request1->addVPack(builder.slice());
    request1->header.meta.erase("accept");
    auto result1 = sendREQUEST(std::move(request1));

    if( result1->statusCode() >=  StatusBadRequest )
        ARANGO_THROW( "ArangoDBGraphAPI", 33, "Error when add collection" );
}

void ArangoDBGraphAPI::addEdgeGraph(const std::string& graphname, const std::string& edgeDefinition)
{
    try {
        auto data = ::arangodb::velocypack::Parser::fromJson(edgeDefinition, &parse_options);

        std::string rqstr = "/_api/gharial/"+graphname+ "/edge";
        auto request1 = createREQUEST(RestVerb::Post, rqstr);
        request1->addVPack(data->slice());
        request1->header.meta.erase("accept");
        auto result1 = sendREQUEST(std::move(request1));

        if( result1->statusCode() >=  StatusBadRequest )
            ARANGO_THROW( "ArangoDBGraphAPI", 34, "The defininition could not be added,"
                                                  "the edge collection is used in an other graph with a different signature" );
    } catch (::arangodb::velocypack::Exception& error )
    {
        JSONIO_LOG << "Velocypack error: " << error.what() << std::endl;
        ARANGO_THROW( "ArangoDBGraphAPI", 31, std::string("Velocypack error: ")+error.what());
    }
}


//----------------------------------------------------------------------------

// Retrive one record from the collection
bool ArangoDBGraphAPI::readRecord( const std::string& graphname, const std::string& colltype,
                                   const std::string& collname, const std::string& documentHandle, std::string& jsonrec )
{
    std::string rid = getId( collname, documentHandle );

    std::string rqstr = "/_api/gharial/"+graphname+ "/"+colltype+"/"+rid;
    auto request = createREQUEST(RestVerb::Get, rqstr  );
    request->header.meta.erase("content-type");
    request->header.meta.erase("accept");
    auto result =  sendREQUEST(std::move(request));
    auto slice = result->slices().front();

    if( result->statusCode() != StatusOK )
    {
        auto errmsg = slice.get("errorMessage").copyString();
        JSONIO_LOG << "Error :" << errmsg << std::endl;
        ARANGO_THROW( "ArangoDBGraphAPI", 35, std::string("Error when try load record: ") + errmsg );
    }
    else
    {
        jsonrec =  slice.get(colltype).toJson(&dump_options);
        JSONIO_LOG << "readRecord :" << jsonrec << std::endl;
        return true;
    }
    //return false;
}


std::string ArangoDBGraphAPI::createRecord( const std::string& graphname, const std::string& colltype,
                                            const std::string& collname, const std::string& jsonrec )
{
    std::string newId = "";

    try{
        auto data = ::arangodb::velocypack::Parser::fromJson(jsonrec, &parse_options);

        std::string rqstr = "/_api/gharial/"+graphname+ "/"+colltype+"/"+collname;
        auto request = createREQUEST(RestVerb::Post, rqstr  );
        request->addVPack(data->slice());
        request->header.meta.erase("accept");
        auto result =  sendREQUEST(std::move(request));
        auto slice1 = result->slices().front();

        if( result->statusCode() >=  StatusBadRequest )
        {
            auto errmsg = slice1.get("errorMessage").copyString();
            JSONIO_LOG << "Error :" << errmsg << std::endl;
            ARANGO_THROW( "ArangoDBGraphAPI", 36, std::string("Error when try create record: ") + errmsg );
        }
        else
        {
            newId= slice1.get(colltype).get("_id").copyString();
            JSONIO_LOG << "createRecord :" << newId << std::endl;
        }
    } catch (::arangodb::velocypack::Exception& error )
    {
        JSONIO_LOG << "Velocypack error: " << error.what() << std::endl;
        ARANGO_THROW( "ArangoDBGraphAPI", 31, std::string("Velocypack error: ")+error.what());
    }

    return newId;
}

std::string ArangoDBGraphAPI::updateRecord( const std::string& graphname, const std::string& colltype,
                                            const std::string& collname, const std::string& documentHandle, const std::string& jsonrec )
{
    std::string newId = "";
    std::string rid = getId( collname, documentHandle );

    try{
        auto data = ::arangodb::velocypack::Parser::fromJson(jsonrec, &parse_options);

        std::string rqstr = "/_api/gharial/"+graphname+ "/"+colltype+"/"+rid;
        auto request = createREQUEST(RestVerb::Put, rqstr  );
        request->addVPack(data->slice());
        request->header.meta.erase("accept");
        auto result =  sendREQUEST(std::move(request));
        auto slice1 = result->slices().front();

        if( result->statusCode() >=  StatusBadRequest )
        {
            auto errmsg = slice1.get("errorMessage").copyString();
            JSONIO_LOG << "Error :" << errmsg << std::endl;
            ARANGO_THROW( "ArangoDBGraphAPI", 37, std::string("Error when try save record: ") + errmsg);
        }
        else
        {
            newId=slice1.get(colltype).get("_id").copyString();
            JSONIO_LOG << "updateRecord :" << newId << std::endl;
        }
    } catch (::arangodb::velocypack::Exception& error )
    {
        JSONIO_LOG << "Velocypack error: " << error.what() << std::endl;
        ARANGO_THROW( "ArangoDBGraphAPI", 31, std::string("Velocypack error: ")+error.what());
    }
    return newId;
}

// delete one record from the collection
bool ArangoDBGraphAPI::deleteRecord( const std::string& graphname, const std::string& colltype,
                                     const std::string& collname, const std::string& documentHandle )
{
    std::string rid = getId( collname, documentHandle );

    std::string rqstr = "/_api/gharial/"+graphname+ "/"+colltype+"/"+rid;
    auto request = createREQUEST(RestVerb::Delete, rqstr  );
    request->header.meta.erase("content-type");
    //request->header.meta.erase("accept");
    auto result =  sendREQUEST(std::move(request));

    if( result->statusCode() >=  StatusBadRequest )
    {
        auto slice = result->slices().front();
        auto errmsg = slice.get("errorMessage").copyString();
        JSONIO_LOG << "Error :" << errmsg << std::endl;
        ARANGO_THROW( "ArangoDBGraphAPI", 38, std::string("Error when try remove record: ") + errmsg );
        //return false;
    }
    return true;
}


} // namespace arangocpp
