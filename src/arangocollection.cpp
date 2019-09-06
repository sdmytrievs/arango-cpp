#include "jsonio/dbquerydef.h"
#include "jsonio/io_settings.h"
#include "arangocollection.h"
#include "arangocurl.h"

namespace jsonio { namespace arangodb {

void ArangoDBAPIBase::resetDBConnection( const ArangoDBConnect& connectData )
{
    if( !(_connect_data != connectData) )
        return;
    _connect_data = connectData;

    /*try{
      testConnection();
    }
  catch(std::exception& e)
  {
     cout <<  "std::exception" << e.what();
  }*/

}


std::unique_ptr<arangodb::HttpMessage> ArangoDBAPIBase::createRequest(
        arangodb::RestVerb verb, std::string const& path, arangodb::StringMap const& parameter )

{
    auto request = arangodb::createRequest(verb, path, parameter );
    return request;
}


// Send a request to the server and wait into a response it received.
std::unique_ptr<arangodb::HttpMessage> ArangoDBAPIBase::sendRequest(std::unique_ptr<arangodb::HttpMessage> rq )
{
    //try{
    DEBUG_OUTPUT( "request", rq );
    auto url = _connect_data.fullURL(rq->header.path);
    arangodb::RequestCurlObject mco( url, _connect_data.user.name, _connect_data.user.password, std::move(rq) );
    auto result = mco.getResponse();
    DEBUG_OUTPUT( "result", result );
    if( !result->isContentTypeVPack() )
        jsonioErr("DBArango002: ", "Illegal content type" );
    if( result->statusCode() == 0 )
        jsonioErr("DBArango005: ", "Server connections error" );
    return result;
    //}catch (arangodb::ErrorCondition& error )
    // {
    //     cout << "Response from server: " << arangodb::to_std::string(error) << endl;
    //     jsonioErr("DBArango001: ", "error response from server", arangodb::to_std::string(error));
    // }
}

void ArangoDBAPIBase::testConnection()
{
    auto request = createRequest(arangodb::RestVerb::Get, "/_api/version");
    auto result = sendRequest(std::move(request));

    auto slice = result->slices().front();
    auto version = slice.get("version").copyString();
    auto server = slice.get("server").copyString();
    std::cout << "You are connected to: " << server << " " << version << std::endl;
}


//----------------------------------------------------------------------------------------

ArangoDBCollectionAPI::~ArangoDBCollectionAPI()
{ }

// Test exist collection
bool ArangoDBCollectionAPI::ExistCollection(const std::string& collname )
{
    std::string qpath  = std::string("/_api/collection/")+collname+"/properties";
    auto request = createRequest(arangodb::RestVerb::Get, qpath );
    auto result = sendRequest(std::move(request));

    return result->statusCode() != arangodb::StatusNotFound;
}

// Create collection if no exist
void ArangoDBCollectionAPI::CreateCollection(const std::string& collname, const std::string& ctype)
{
    // test exist collection
    if( ExistCollection( collname ) )
        return;

    ::arangodb::velocypack::Builder builder;
    builder.openObject();
    builder.add("name" , ::arangodb::velocypack::Value(std::string(collname)));
    if( ctype == "edge")
        builder.add("type" , ::arangodb::velocypack::Value(3));
    builder.close();

    auto request1 = createRequest(arangodb::RestVerb::Post, std::string("/_api/collection") );
    request1->addVPack(builder.slice());
    auto result1 = sendRequest(std::move(request1));

    if( result1->statusCode() != arangodb::StatusOK )
        jsonioErr( "DBArango003: ", "Error when create collection" );
}

// Drops the collection identified by collection-name
void ArangoDBCollectionAPI::DropCollection(const std::string& collname )
{
    // test exist collection
    if( !ExistCollection( collname ) )
        return;

    std::string qpath  = std::string("/_api/collection/")+collname;
    auto request = createRequest(arangodb::RestVerb::Delete, qpath );
    auto result = sendRequest(std::move(request));

    if( result->statusCode() != arangodb::StatusOK )
        jsonioErr( "DBArango010: ", "Error when drop collection" );
}

std::set<std::string> ArangoDBCollectionAPI::getCollectionNames( TAbstractDBDriver::CollectionTypes ctype )
{
    std::set<std::string> collnames;
    auto request = createRequest(arangodb::RestVerb::Get, "/_api/collection"  );
    auto result =  sendRequest(std::move(request));

    if( result->statusCode() == arangodb::StatusOK )
    {
        auto slice = result->slices().front();
        auto collst = slice.get("result");

        auto numb = collst.length();
        for( uint ii=0; ii<numb; ii++ )
        {
            if( collst[ii].get("isSystem").getBool() )
                continue;
            if(  ctype&TAbstractDBDriver::clVertex && collst[ii].get("type").getInt() == 2 )  // 2: document collection (normal case)
                collnames.insert(collst[ii].get("name").copyString());
            else
                if( ctype&TAbstractDBDriver::clEdge && collst[ii].get("type").getInt() == 3 )   // 3: edges collection
                    collnames.insert(collst[ii].get("name").copyString());
        }
    }

    return collnames;
}

bool ArangoDBCollectionAPI::ReadRecord( const std::string& collname,
                                        const std::string& key, std::string& jsonrec )
{
    std::string rid = getId( collname, key );

    std::string rqstr = "/_api/document/"+rid;
    auto request = createRequest(arangodb::RestVerb::Get, rqstr  );
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
        jsonrec =  slice.toJson(&_dumpoptions);
        JSONIO_LOG << "readRecord :" << jsonrec << std::endl;
        return true;
    }
    //return false;
}

std::string ArangoDBCollectionAPI::CreateRecord( const std::string& collname, const std::string& jsonrec )
{
    std::string newId = "";

    try{
        auto data = ::arangodb::velocypack::Parser::fromJson( jsonrec, &_parseoptions);

        std::string rqstr = std::string("/_api/document/")+collname;
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
            newId=slice1.get("_id").copyString();
            JSONIO_LOG << "createRecord :" << newId << std::endl;
        }
    } catch (::arangodb::velocypack::Exception& error )
    {
        std::cout << "Velocypack error: " << error.what() << std::endl;
        jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
    }

    return newId;
}

std::string ArangoDBCollectionAPI::UpdateRecord( const std::string& collname,
                                                 const std::string& key, const std::string& jsonrec )
{
    std::string newId = "";
    std::string rid = getId( collname, key );

    try{
        auto data = ::arangodb::velocypack::Parser::fromJson(jsonrec);

        std::string rqstr = std::string("/_api/document/")+rid;
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
            newId=slice1.get("_id").copyString();
            JSONIO_LOG << "updateRecord :" << newId << std::endl;
        }
    } catch (::arangodb::velocypack::Exception& error )
    {
        std::cout << "Velocypack error: " << error.what() << std::endl;
        jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
    }
    return newId;
}

bool ArangoDBCollectionAPI::DeleteRecord( const std::string& collname, const std::string& key )
{
    std::string rid = getId( collname, key );

    std::string rqstr = "/_api/document/"+rid;
    auto request = createRequest(arangodb::RestVerb::Delete, rqstr  );
    request->header.meta.erase("accept");
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

bool ArangoDBCollectionAPI::ExistsRecord( const std::string& collname,
                                          const std::string& key )
{
    std::string rid = getId( collname, key );

    std::string rqstr = "/_api/document/"+rid;
    auto request = createRequest(arangodb::RestVerb::Head, rqstr  );
    auto result =  sendRequest(std::move(request));

    if( result->statusCode() == arangodb::StatusOK )
        return true;
    return false;
}

//-------------------------------------------------------------------

std::unique_ptr<arangodb::HttpMessage> ArangoDBCollectionAPI::createSimpleAllRequest( const std::string& collname )

{
    ::arangodb::velocypack::Builder builder;
    builder.openObject();
    builder.add("collection" , ::arangodb::velocypack::Value(collname));
    builder.add("batchSize" , ::arangodb::velocypack::Value(_batchSize));
    builder.close();

    auto request = createRequest(arangodb::RestVerb::Put, std::string("/_api/simple/all"));
    request->addVPack(builder.slice());
    return request;
}


std::unique_ptr<arangodb::HttpMessage> ArangoDBCollectionAPI::createByExampleRequest
( const std::string& collname,  const std::string& jsontempl  )

{
    try{
        auto data = ::arangodb::velocypack::Parser::fromJson(jsontempl);

        ::arangodb::velocypack::Builder builder;
        builder.openObject();
        builder.add("collection" , ::arangodb::velocypack::Value(collname) );
        builder.add("example" , data->slice() );
        builder.add("batchSize" , ::arangodb::velocypack::Value(_batchSize));
        builder.close();

        auto request = createRequest(arangodb::RestVerb::Put, std::string("/_api/simple/by-example"));
        request->addVPack(builder.slice());
        request->header.meta.erase("accept");
        return request;
    }
    catch (::arangodb::velocypack::Exception& error )
    {
        std::cout << "Velocypack error: " << error.what() << std::endl;
        jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
    }
}


std::unique_ptr<arangodb::HttpMessage> ArangoDBCollectionAPI::createAQLRequest
( const std::string&,  const DBQueryData& query  )

{
    try{

        auto aqlQuery = query.getQueryString();
        if( aqlQuery.find("RETURN") == std::string::npos )
            aqlQuery += query.generateReturn();            // only with "u"

        ::arangodb::velocypack::Builder builder;
        builder.openObject();
        //builder.add("collection" , VPackValue(collname) );
        builder.add("query" , ::arangodb::velocypack::Value( aqlQuery) );
        builder.add("count" , ::arangodb::velocypack::Value(true));
        if( !query.getBindVars().empty() )
        {
            auto data = ::arangodb::velocypack::Parser::fromJson(query.getBindVars());
            builder.add("bindVars" , data->slice() );
            builder.add("batchSize" , ::arangodb::velocypack::Value(_batchSize*2));
        } else
            builder.add("batchSize" , ::arangodb::velocypack::Value(_batchSize));
        builder.close();

        auto request = createRequest(arangodb::RestVerb::Post, std::string("/_api/cursor"));
        request->addVPack(builder.slice());
        return request;
    }
    catch (::arangodb::velocypack::Exception& error )
    {
        std::cout << "Velocypack error: " << error.what() << std::endl;
        jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
    }
}

void ArangoDBCollectionAPI::extractData( const ::arangodb::velocypack::Slice& sresult,  jsonio::SetReadedFunction setfnc )
{
    auto numb = sresult.length();
    for( uint ii=0; ii<numb; ii++ )
        setfnc( sresult[ii].toJson(&_dumpoptions) );
}

void ArangoDBCollectionAPI::extractData( const ::arangodb::velocypack::Slice& sresult,  jsonio::SetReadedFunctionKey setfnc )
{
    auto numb = sresult.length();
    for( uint ii=0; ii<numb; ii++ )
    {
        auto Id=sresult[ii].get("_id").copyString();
        setfnc( sresult[ii].toJson(&_dumpoptions), Id );
    }
}

void ArangoDBCollectionAPI::SelectQuery( const std::string& collname,
                                         const jsonio::DBQueryData& query,  jsonio::SetReadedFunction setfnc )
{
    std::unique_ptr<arangodb::HttpMessage> request;
    bool usejson = true;

    switch( query.getType() )
    {
    case DBQueryData::qTemplate:
        request = createByExampleRequest( collname,  query.getQueryString()  );
        usejson = true;
        break;
    case DBQueryData::qUndef:
    case DBQueryData::qAll:
        request = createSimpleAllRequest( collname );
        break;
    case DBQueryData::qEdgesAll:
    case DBQueryData::qEdgesFrom:
    case DBQueryData::qEdgesTo:
    case DBQueryData::qAQL:
        request = createAQLRequest( collname,  query  );
        break;
    default:
        jsonioErr("DBArango006: ", "Illegal query type ");
        //break;
    }

    auto result = sendRequest(std::move(request));
    if( result->statusCode() >=  arangodb::StatusBadRequest )
        return;

    auto slice = result->slices().front();
    auto hasMore = slice.get("hasMore").getBool();
    auto query_result = slice.get("result");
    extractData( query_result, setfnc );

    // Read data from cursor
    while( hasMore )
    {
        std::string id=slice.get("id").copyString();
        request = createRequest(arangodb::RestVerb::Put, std::string("/_api/cursor/")+id);
        if( usejson )
            request->header.meta.erase("accept");
        result = sendRequest(std::move(request));
        if( result->statusCode() >=  arangodb::StatusBadRequest )
            return;  //need close cursor???
        slice = result->slices().front();
        hasMore = slice.get("hasMore").getBool();
        query_result = slice.get("result");
        extractData( query_result, setfnc );
    }
}

void ArangoDBCollectionAPI::AllQueryFields( bool isComlexFields, const std::string& collname,
                                            const std::set<std::string>& queryFields,  jsonio::SetReadedFunctionKey setfnc )
{
    std::unique_ptr<arangodb::HttpMessage> request;
    if( isComlexFields )
        request = createSimpleAllRequest( collname );
    else
    {    std::string AQLquery = "FOR u IN ";
        AQLquery += collname;
        DBQueryData query( AQLquery, DBQueryData::qAQL);
        query.setQueryFields(queryFields);

        request = createAQLRequest( collname,  query  );
    }
    auto result = sendRequest(std::move(request));
    if( result->statusCode() >=  arangodb::StatusBadRequest )
        return;

    auto slice = result->slices().front();
    auto hasMore = slice.get("hasMore").getBool();
    auto query_result = slice.get("result");
    extractData( query_result, setfnc );

    // Read data from cursor
    while( hasMore )
    {
        std::string id=slice.get("id").copyString();
        request = createRequest(arangodb::RestVerb::Put, std::string("/_api/cursor/")+id);
        result = sendRequest(std::move(request));
        if( result->statusCode() >=  arangodb::StatusBadRequest )
            return;  //need close cursor???
        slice = result->slices().front();
        hasMore = slice.get("hasMore").getBool();
        query_result = slice.get("result");
        extractData( query_result, setfnc );
    }
}

void ArangoDBCollectionAPI::SearchEdgesToFrom(const std::string& ,
                                              const jsonio::DBQueryData& query,  jsonio::SetReadedFunction setfnc)
{
    std::string startVertex;
    std::string edgeCollections;

    // get id from query
    try{
        // might be jsonio extract field !!!
        auto data = ::arangodb::velocypack::Parser::fromJson(query.getQueryString());
        auto slice = data->slice();
        auto slicelabel = slice.get("edgeCollections");
        if( !slicelabel.isNone() )
            edgeCollections = slicelabel.copyString();
        startVertex = slice.get("startVertex").copyString();
    }
    catch (::arangodb::velocypack::Exception& error )
    {
        std::cout << "Velocypack error: " << error.what() << std::endl;
        jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
    }

    // build all edges collections
    if( edgeCollections.empty() )
    {
        auto edgesexist = getCollectionNames( TAbstractDBDriver::clEdge );
        auto edgesdefined = ioSettings().Schema()->getEdgesAllDefined();
        auto edges = getSubset( edgesdefined, edgesexist );
        for( auto edgecoll: edges)
        {
            if( !edgeCollections.empty())
                edgeCollections += ", ";
            edgeCollections += edgecoll;
        }
    }

    std::string AQLquery = "FOR v,e IN 1..1 ";
    switch( query.getType() )
    {
    case DBQueryData::qEdgesAll:
        AQLquery += "ANY '"; break;
    case DBQueryData::qEdgesFrom:
        AQLquery += "OUTBOUND '"; break;
    case DBQueryData::qEdgesTo:
        AQLquery += "INBOUND '"; break;
    }
    AQLquery +=  startVertex +"' \n";
    AQLquery +=  edgeCollections + query.generateReturn("e"); //" \nRETURN e";
    SelectQuery( edgeCollections, DBQueryData( AQLquery, DBQueryData::qAQL),  setfnc );
}


void ArangoDBCollectionAPI::SearchEdgesToFromOld(const std::string& ,
                                                 const jsonio::DBQueryData& query,  jsonio::SetReadedFunction setfnc)
{
    std::string edjecollname;
    try{ // might be jsonio extract field !!!
        auto data = ::arangodb::velocypack::Parser::fromJson(query.getQueryString());
        auto slice = data->slice();
        auto slicelabel = slice.get("_label");
        if( !slicelabel.isNone() )
            edjecollname = slicelabel.copyString();
    }
    catch (::arangodb::velocypack::Exception& error )
    {
        std::cout << "Velocypack error: " << error.what() << std::endl;
        jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
    }

    if( !edjecollname.empty() )
    {
        SelectQuery( edjecollname, query,  setfnc );
    }
    else
    { // get edjes collections names
        auto edges = getCollectionNames( TAbstractDBDriver::clEdge );
        for( auto edgecoll: edges)
            SelectQuery( edgecoll, query,  setfnc );
    }
}

void ArangoDBCollectionAPI::RemoveByExample( const std::string& collname,  const std::string& jsontempl  )
{
    try{
        auto data = ::arangodb::velocypack::Parser::fromJson(jsontempl);

        ::arangodb::velocypack::Builder builder;
        builder.openObject();
        builder.add("collection" , ::arangodb::velocypack::Value(collname) );
        builder.add("example" , data->slice() );
        builder.close();

        auto request = createRequest(arangodb::RestVerb::Put, std::string("/_api/simple/remove-by-example"));
        request->addVPack(builder.slice());
        auto result =  sendRequest(std::move(request));
        auto slice1 = result->slices().front();

        if( result->statusCode() >=  arangodb::StatusBadRequest )
        {
            auto errmsg = slice1.get("errorMessage").copyString();
            JSONIO_LOG << "Error removeByExampleRequest:" << errmsg << std::endl;
        }
        else
        {
            JSONIO_LOG <<  collname << " - deleted :" << slice1.get("deleted").getInt() << std::endl;
        }
    }
    catch (::arangodb::velocypack::Exception& error )
    {
        std::cout << "Velocypack error: " << error.what() << std::endl;
        jsonioErr("DBArango004: ", "Velocypack error: ", error.what());
    }
}


void ArangoDBCollectionAPI::RemoveEdges(const std::string& collname, const std::string& vertexkey )
{
    std::string vertexid= getId( collname, vertexkey );
    std::string to_templ = std::string("{\"_to\": \"")+ vertexid + "\" }";
    std::string from_templ = std::string("{\"_from\": \"")+ vertexid + "\" }";

    // get edges collections names
    auto edges = getCollectionNames( TAbstractDBDriver::clEdge );
    for( auto edgecoll: edges)
    {
        RemoveByExample( edgecoll,  to_templ  );
        RemoveByExample( edgecoll,  from_templ  );
    }

    /*

   db._query(aql`
     FOR s IN ${subtitlesEdges}
       FILTER s._to == ${_id}
       REMOVE s IN ${subtitlesEdges}
   `)

   subtitlesCollection.remove({ _key })
   */

}

void ArangoDBCollectionAPI::CollectQuery( const std::string& collname,
                                          const std::string& fpath, std::vector<std::string>& values )
{
    /* other
  FOR doc IN collection
  COLLECT status = doc.status, age = doc.age,
  RETURN { status, age }
  */
    std::string aqlQuery = "FOR doc IN ";
    aqlQuery += collname;
    aqlQuery += " RETURN DISTINCT doc.";
    aqlQuery += fpath;

    SetReadedFunction setfnc = [&]( const std::string& jsondata)
    {
        if(jsondata != "null")
            values.push_back(jsondata);
        JSONIO_LOG << jsondata << std::endl;
    };
    DBQueryData query(aqlQuery,DBQueryData::qAQL );
    SelectQuery( collname, query,  setfnc );
}

// Fetches multiple documents by their keys
void ArangoDBCollectionAPI::LookupByKeys( const std::string& collname,
                                          const std::vector<std::string>& keys,  SetReadedFunction setfnc )
{
    ::arangodb::velocypack::Builder builder;
    builder.openObject();
    builder.add("collection" , ::arangodb::velocypack::Value(collname) );
    builder.add("keys" ,  ::arangodb::velocypack::Value(::arangodb::velocypack::ValueType::Array) );
    for( auto key: keys )
        builder.add( ::arangodb::velocypack::Value(key));
    builder.close();
    builder.close();

    auto request = createRequest(arangodb::RestVerb::Put, std::string("/_api/simple/lookup-by-keys"));
    request->addVPack(builder.slice());
    //request->header.meta.erase("accept");

    auto result = sendRequest(std::move(request));
    if( result->statusCode() >=  arangodb::StatusBadRequest )
        return;

    auto slice = result->slices().front();
    auto query_result = slice.get("documents");
    extractData( query_result, setfnc );
}

// Removes multiple documents by their keys
void ArangoDBCollectionAPI::RemoveByKeys( const std::string& collname,  const std::vector<std::string>& ids  )
{
    std::vector<std::string> keys;
    // ids to keys
    for( auto idit: ids )
        keys.push_back( getKey(  collname, idit ) );

    ::arangodb::velocypack::Builder builder;
    builder.openObject();
    builder.add("collection" , ::arangodb::velocypack::Value(collname) );
    builder.add("keys" ,  ::arangodb::velocypack::Value(::arangodb::velocypack::ValueType::Array) );
    for( auto key: keys )
        builder.add( ::arangodb::velocypack::Value(key));
    builder.close();
    builder.close();

    auto request = createRequest(arangodb::RestVerb::Put, std::string("/_api/simple/remove-by-keys"));
    request->addVPack(builder.slice());
    auto result =  sendRequest(std::move(request));
    auto slice1 = result->slices().front();

    if( result->statusCode() >=  arangodb::StatusBadRequest )
    {
        auto errmsg = slice1.get("errorMessage").copyString();
        JSONIO_LOG << "Error removeByKeys:" << errmsg << std::endl;
    }
    else
    {
        JSONIO_LOG <<  collname << " - removed :" << slice1.get("removed").getInt() <<
                       " - ignored :" << slice1.get("ignored").getInt() << std::endl;
    }
}

} }
