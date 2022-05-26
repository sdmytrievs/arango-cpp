#include <locale>
#include "jsonarango/arangocollection.h"
#include "jsonarango/arangoexception.h"
#include "arangodetail.h"
#include "curlobjects_pool.h"


namespace arangocpp {


ArangoDBAPIBase::ArangoDBAPIBase(const ArangoDBConnection &connectData)
{
    dump_options.unsupportedTypeBehavior = ::arangodb::velocypack::Options::NullifyUnsupportedType;
    parse_options.validateUtf8Strings = true;
    resetDBConnection(connectData);
}

void ArangoDBAPIBase::resetDBConnection( const ArangoDBConnection& connectData )
{
    if( !(connect_data != connectData) ) {
        return;
    }
    connect_data = connectData;
    try{
        testConnection();
    }
    catch(std::exception& e)
    {
        arango_logger->warn("DB connection error: {}", e.what());
    }
}

/*
 * The key must be a string value.
 * Numeric keys are not allowed, but any numeric value can be put into a string and
 *  can then be used as document key.
 * The key must be at least 1 byte and at most 254 bytes long.
 * Empty keys are disallowed when specified (though it may be valid to completely
 *  omit the _key attribute from a document)
 * It must consist of the letters a-z (lower or upper case), the digits 0-9 or
 *  any of the following punctuation characters: "_-:.@()+,=;$!*'%"
 * Any other characters, especially multi-byte UTF-8 sequences,
 * whitespace or punctuation characters cannot be used inside key values
 */

static const std::string  KeyPunctuationCharacters = "_-:.@()+,=;$!*'";

std::string ArangoDBAPIBase::sanitization(const std::string &documentHandle)
{
    std::string legal_key, input_key = documentHandle;
    bool first = true;

    detail::trim(input_key);
    for( const auto c: input_key ) {
        if( std::isalnum(c, std::locale()) ||
                ( KeyPunctuationCharacters.find(c) != std::string::npos ) ) {
            first = true;
            legal_key +=c;
        }
        else if( first ) {
            legal_key +='_';
            first = false;
        }
    }
    return legal_key.substr(0, 254);
}


std::unique_ptr<HttpMessage> ArangoDBAPIBase::createREQUEST(
        RestVerb verb, std::string const& path, StringMap const& parameter )

{
    auto request = createRequest(verb, path, parameter );
    return request;
}


// Send a request to the server and wait into a response it received.
std::unique_ptr<HttpMessage> ArangoDBAPIBase::sendREQUEST(std::unique_ptr<HttpMessage> rq )
{
    auto url = connect_data.fullURL(rq->header.path);

    if(arango_logger->should_log(spdlog::level::debug)) {
        arango_logger->debug("DBAPIBase REQUEST: {}", to_string(*(rq)));
    }
    auto curl_object = pool_connect().get_resource();
    curl_object->setConnectData( connect_data.user.name, connect_data.user.password );
    curl_object->sendRequest( url, std::move(rq) );
    auto result = curl_object->getResponse();
    pool_connect().return_resource( std::move(curl_object) );
    if(arango_logger->should_log(spdlog::level::debug)) {
        arango_logger->debug("DBAPIBase RESPONSE: {}", to_string(*(result)));
    }

    if( !result->isContentTypeVPack() ) {
        ARANGO_THROW( "ArangoDBAPIBase", 3, "Illegal content type" );
    }
    if( result->statusCode() == 0 ) {
        ARANGO_THROW( "ArangoDBAPIBase", 2, "Server connections error" );
    }
    return result;
}

void ArangoDBAPIBase::testConnection()
{
    auto request = createREQUEST(RestVerb::Get, "/_api/version");
    auto result = sendREQUEST(std::move(request));

    auto slice = result->slices().front();
    auto version = slice.get("version").copyString();
    auto server = slice.get("server").copyString();
    arango_logger->info("You are connected to: {} {}", server, version);
}

//----------------------------------------------------------------------------------------

ArangoDBCollectionAPI::~ArangoDBCollectionAPI()
{}

// Test exist collection
bool ArangoDBCollectionAPI::existCollection(const std::string& collname )
{
    //std::string qpath  = std::string("/_api/collection/")+collname+"/properties";
    std::string qpath  = std::string("/_api/collection/")+collname;
    auto request = createREQUEST(RestVerb::Get, qpath );
    auto result = sendREQUEST(std::move(request));

    int status = 5; //deleted
    if(result->statusCode() != StatusNotFound ) {
        auto slice = result->slices().front();
        status = slice.get("status").getInt();
    }
    return ( status != 5 );
}

// Create collection if no exist
void ArangoDBCollectionAPI::createCollection(const std::string& collname, const std::string& ctype)
{
    // test exist collection
    if( existCollection( collname ) ) {
        return;
    }
    ::arangodb::velocypack::Builder builder;
    builder.openObject();
    builder.add("name" , ::arangodb::velocypack::Value(std::string(collname)));
    if( ctype == "edge") {
        builder.add("type" , ::arangodb::velocypack::Value(3));
    }
    builder.close();

    auto request1 = createREQUEST(RestVerb::Post, std::string("/_api/collection") );
    request1->addVPack(builder.slice());
    auto result1 = sendREQUEST(std::move(request1));

    if( result1->statusCode() != StatusOK ) {
        auto slice = result1->slices().front();
        auto errmsg = slice.get("errorMessage").copyString();
        ARANGO_THROW( "ArangoDBCollectionAPI", 12, std::string("Error when create collection: ") + errmsg );
    }
}

// Drops the collection identified by collection-name
void ArangoDBCollectionAPI::dropCollection(const std::string& collname )
{
    // test exist collection
    if( !existCollection( collname ) ) {
        return;
    }
    std::string qpath  = std::string("/_api/collection/")+collname;
    auto request = createREQUEST(RestVerb::Delete, qpath );
    auto result = sendREQUEST(std::move(request));

    if( result->statusCode() != StatusOK ) {
        auto slice = result->slices().front();
        auto errmsg = slice.get("errorMessage").copyString();
        ARANGO_THROW( "ArangoDBCollectionAPI", 13, std::string("Error when try drop collection: ") + errmsg );
    }
}

std::set<std::string> ArangoDBCollectionAPI::collectionNames( CollectionTypes ctype )
{
    std::set<std::string> collnames;
    auto request = createREQUEST(RestVerb::Get, "/_api/collection"  );
    auto result =  sendREQUEST(std::move(request));

    if( result->statusCode() == StatusOK ) {
        auto slice = result->slices().front();
        auto collst = slice.get("result");

        auto numb = collst.length();
        for( size_t ii=0; ii<numb; ii++ ) {
            if( collst[ii].get("isSystem").getBool() ) {
                continue;
            }
            if(  ctype&CollectionTypes::Vertex && collst[ii].get("type").getInt() == 2 ) {
                // 2: document collection (normal case)
                collnames.insert(collst[ii].get("name").copyString());
            }
            else if( ctype&CollectionTypes::Edge && collst[ii].get("type").getInt() == 3 ) {
                // 3: edges collection
                collnames.insert(collst[ii].get("name").copyString());
            }
        }
    }
    return collnames;
}

bool ArangoDBCollectionAPI::readDocument( const std::string& collname,
                                          const std::string& key, std::string& jsonrec )
{
    std::string rid = getId( collname, key );

    std::string rqstr = "/_api/document/"+rid;
    auto request = createREQUEST(RestVerb::Get, rqstr  );
    auto result =  sendREQUEST(std::move(request));
    auto slice = result->slices().front();

    if( result->statusCode() != StatusOK ) {
        auto errmsg = slice.get("errorMessage").copyString();
        ARANGO_THROW( "ArangoDBCollectionAPI", 14, std::string("Error when try load record: ") + errmsg);
    }
    else {
        jsonrec =  slice.toJson(&dump_options);
        arango_logger->debug("Readed record:\n {}", jsonrec);
        return true;
    }
    //return false;
}

std::string ArangoDBCollectionAPI::createDocument( const std::string& collname, const std::string& jsonrec )
{
    std::string newId = "";
    try {
        auto data = ::arangodb::velocypack::Parser::fromJson( jsonrec, &parse_options);

        std::string rqstr = std::string("/_api/document/")+collname;
        auto request = createREQUEST(RestVerb::Post, rqstr  );
        request->addVPack(data->slice());
        request->header.meta.erase("accept");
        auto result =  sendREQUEST(std::move(request));
        auto slice1 = result->slices().front();

        if( result->statusCode() >=  StatusBadRequest ) {
            auto errmsg = slice1.get("errorMessage").copyString();
            ARANGO_THROW( "ArangoDBCollectionAPI", 15, std::string("Error when try create record: ") + errmsg);
        }
        else {
            newId=slice1.get("_id").copyString();
            arango_logger->debug("Created record: {}", newId);
        }
    }
    catch (::arangodb::velocypack::Exception& error) {
        ARANGO_THROW( "ArangoDBCollectionAPI", 11, std::string("Velocypack error: ")+error.what());
    }
    return newId;
}

std::string ArangoDBCollectionAPI::updateDocument( const std::string& collname,
                                                   const std::string& key, const std::string& jsonrec )
{
    std::string newId = "";
    std::string rid = getId( collname, key );
    try {
        auto data = ::arangodb::velocypack::Parser::fromJson(jsonrec, &parse_options);

        std::string rqstr = std::string("/_api/document/")+rid;
        auto request = createREQUEST(RestVerb::Put, rqstr  );
        request->addVPack(data->slice());
        request->header.meta.erase("accept");
        auto result =  sendREQUEST(std::move(request));
        auto slice1 = result->slices().front();

        if( result->statusCode() >=  StatusBadRequest ) {
            auto errmsg = slice1.get("errorMessage").copyString();
            ARANGO_THROW( "ArangoDBCollectionAPI", 16, std::string("Error when try save record: ") + errmsg);
        }
        else {
            newId=slice1.get("_id").copyString();
            arango_logger->debug("Updated record: {}", newId);
        }
    }
    catch (::arangodb::velocypack::Exception& error ) {
        ARANGO_THROW( "ArangoDBCollectionAPI", 11, std::string("Velocypack error: ")+error.what());
    }
    return newId;
}

bool ArangoDBCollectionAPI::deleteDocument( const std::string& collname, const std::string& key )
{
    std::string rid = getId( collname, key );

    std::string rqstr = "/_api/document/"+rid;
    auto request = createREQUEST(RestVerb::Delete, rqstr  );
    //request->header.meta.erase("accept");
    auto result =  sendREQUEST(std::move(request));

    if( result->statusCode() >=  StatusBadRequest ) {
        auto slice = result->slices().front();
        auto errmsg = slice.get("errorMessage").copyString();
        ARANGO_THROW( "ArangoDBCollectionAPI", 17, std::string("Error when try remove record: ") + errmsg);
        //return false;
    }
    return true;
}

bool ArangoDBCollectionAPI::existsDocument( const std::string& collname,
                                            const std::string& key )
{
    std::string rid = getId( collname, key );

    std::string rqstr = "/_api/document/"+rid;
    auto request = createREQUEST(RestVerb::Head, rqstr  );
    auto result =  sendREQUEST(std::move(request));

    if( result->statusCode() == StatusOK ) {
        return true;
    }
    return false;
}

//-------------------------------------------------------------------

std::unique_ptr<HttpMessage> ArangoDBCollectionAPI::createSimpleAllRequest( const std::string& collname )

{
    ::arangodb::velocypack::Builder builder;
    builder.openObject();
    builder.add("collection" , ::arangodb::velocypack::Value(collname));
    builder.add("batchSize" , ::arangodb::velocypack::Value(batch_size));
    builder.close();

    auto request = createREQUEST(RestVerb::Put, std::string("/_api/simple/all"));
    request->addVPack(builder.slice());
    return request;
}


std::unique_ptr<HttpMessage> ArangoDBCollectionAPI::createByExampleRequest(
        const std::string& collname,  const std::string& jsontempl  )
{
    try {
        auto data = ::arangodb::velocypack::Parser::fromJson(jsontempl, &parse_options);

        ::arangodb::velocypack::Builder builder;
        builder.openObject();
        builder.add("collection" , ::arangodb::velocypack::Value(collname) );
        builder.add("example" , data->slice() );
        builder.add("batchSize" , ::arangodb::velocypack::Value(batch_size));
        builder.close();

        auto request = createREQUEST(RestVerb::Put, std::string("/_api/simple/by-example"));
        request->addVPack(builder.slice());
        //request->header.meta.erase("accept");
        return request;
    }
    catch (::arangodb::velocypack::Exception& error ) {
        ARANGO_THROW( "ArangoDBCollectionAPI", 11, std::string("Velocypack error: ")+error.what());
    }
}


std::unique_ptr<HttpMessage> ArangoDBCollectionAPI::createAQLRequest(
        const std::string&,  const ArangoDBQuery& query  )
{
    try {

        auto aqlQuery = query.queryString();
        if( aqlQuery.find("RETURN") == std::string::npos ) {
            aqlQuery += query.generateRETURN( false );
            // only with "u"
        }

        ::arangodb::velocypack::Builder builder;
        builder.openObject();
        //builder.add("collection" , VPackValue(collname) );
        builder.add("query" , ::arangodb::velocypack::Value( aqlQuery) );
        builder.add("count" , ::arangodb::velocypack::Value(true));
        if( !query.bindVars().empty() ) {
            auto data = ::arangodb::velocypack::Parser::fromJson(query.bindVars(), &parse_options);
            builder.add("bindVars" , data->slice() );
            builder.add("batchSize" , ::arangodb::velocypack::Value(batch_size*2));
        }
        else {
            builder.add("batchSize" , ::arangodb::velocypack::Value(batch_size));
        }
        if( !query.options().empty() ) {
            auto data = ::arangodb::velocypack::Parser::fromJson(query.options(), &parse_options);
            builder.add("options" , data->slice() );
        }
        builder.close();

        auto request = createREQUEST(RestVerb::Post, std::string("/_api/cursor"));
        request->addVPack(builder.slice());
        return request;
    }
    catch (::arangodb::velocypack::Exception& error ) {
        ARANGO_THROW( "ArangoDBCollectionAPI", 11, std::string("Velocypack error: ")+error.what());
    }
}

std::unique_ptr<HttpMessage> ArangoDBCollectionAPI::createEdgeRequest(const std::string &collname, const ArangoDBQuery& query)
{
    std::string startVertex;
    std::string edgeCollections;

    // get id from query
    try {
        // might be jsonio extract field !!!
        auto data = ::arangodb::velocypack::Parser::fromJson(query.queryString(), &parse_options);
        auto slice = data->slice();
        auto slicelabel = slice.get("edgeCollections");
        if( !slicelabel.isNone() ) {
            edgeCollections = slicelabel.copyString();
        }
        startVertex = slice.get("startVertex").copyString();
    }
    catch (::arangodb::velocypack::Exception& error ) {
        ARANGO_THROW( "ArangoDBCollectionAPI", 11, std::string("Velocypack error: ")+error.what());
    }

    auto querytofrom =  queryEdgesToFrom( query.type(), startVertex, edgeCollections );
    return createAQLRequest( collname,  querytofrom  );
}

void ArangoDBCollectionAPI::extractData( const ::arangodb::velocypack::Slice& sresult,  FetchingDocumentCallback setfnc )
{
    auto numb = sresult.length();
    for( size_t ii=0; ii<numb; ii++ ) {
        setfnc( sresult[ii].toJson(&dump_options) );
    }
}

void ArangoDBCollectionAPI::extractData( const ::arangodb::velocypack::Slice& sresult,  FetchingDocumentIdCallback setfnc )
{
    auto numb = sresult.length();
    for( size_t ii=0; ii<numb; ii++ ) {
        auto Id=sresult[ii].get("_id").copyString();
        setfnc( sresult[ii].toJson(&dump_options), std::move(Id) );
    }
}

// Create select edges from data
ArangoDBQuery ArangoDBCollectionAPI::queryEdgesToFrom( ArangoDBQuery::QueryType atype, const std::string& startVertex,
                                                       const std::string& edgeCollectionsdefault )
{
    std::string edgeCollections{edgeCollectionsdefault};

    // build all edges collections
    if( edgeCollections.empty() ) {
        auto edgesexist = collectionNames( CollectionTypes::Edge );
        std::vector<std::string> edges;
        if( ArangoDBConnection::full_list_of_edges.empty() ) {
            edges.insert(edges.begin(), edgesexist.begin(), edgesexist.end());
        }
        else {
            edges = detail::getSubset( ArangoDBConnection::full_list_of_edges, edgesexist );
        }
        for( const auto& edgecoll: edges) {
            if( !edgeCollections.empty()) {
                edgeCollections += ", ";
            }
            edgeCollections += edgecoll;
        }
    }
    std::string AQLquery = "FOR v,e IN 1..1 ";
    switch( atype ) {
    case ArangoDBQuery::EdgesAll:
        AQLquery += "ANY '"; break;
    case ArangoDBQuery::EdgesFrom:
        AQLquery += "OUTBOUND '"; break;
    case ArangoDBQuery::EdgesTo:
        AQLquery += "INBOUND '"; break;
    default:
        ARANGO_THROW( "ArangoDBCollectionAPI", 19, std::string("Illegal query type"));
    }
    AQLquery +=  startVertex +"' \n";
    AQLquery +=  edgeCollections + ArangoDBQuery::generateRETURN( true, {}, "e"); //" \nRETURN e";
    return ArangoDBQuery( AQLquery, ArangoDBQuery::AQL);
}


// Possible work with cursor
void ArangoDBCollectionAPI::selectQuery( const std::string& collname,
                                         const ArangoDBQuery& query,  FetchingDocumentCallback setfnc )
{
    std::unique_ptr<HttpMessage> request;
    //bool usejson = true;

    switch( query.type() ) {
    case ArangoDBQuery::Template:
        request = createByExampleRequest( collname,  query.queryString()  );
        //usejson = true;
        break;
    case ArangoDBQuery::All:
        request = createSimpleAllRequest( collname );
        break;
    case ArangoDBQuery::EdgesAll:
    case ArangoDBQuery::EdgesFrom:
    case ArangoDBQuery::EdgesTo:
        request = createEdgeRequest( collname,  query  );
        break;
    case ArangoDBQuery::AQL:
        request = createAQLRequest( collname,  query  );
        break;
    default:
        ARANGO_THROW( "ArangoDBCollectionAPI", 18, "Illegal query type ");
        //break;
    }

    auto result = sendREQUEST(std::move(request));
    if( result->statusCode() >=  StatusBadRequest ) {
        return;
    }

    auto slice = result->slices().front();
    auto hasMore = slice.get("hasMore").getBool();
    auto query_result = slice.get("result");
    extractData( query_result, setfnc );

    // Read data from cursor
    while( hasMore ) {
        std::string id=slice.get("id").copyString();
        request = createREQUEST(RestVerb::Put, std::string("/_api/cursor/")+id);
        //if( usejson )
        //    request->header.meta.erase("accept");
        result = sendREQUEST(std::move(request));
        if( result->statusCode() >=  StatusBadRequest ) {
            return;  //need close cursor???
        }
        slice = result->slices().front();
        hasMore = slice.get("hasMore").getBool();
        query_result = slice.get("result");
        extractData( query_result, setfnc );
    }
}

std::vector<std::string> ArangoDBCollectionAPI::selectQuery( const std::string& collname,
                                                             const ArangoDBQuery& query )
{
    std::vector<std::string> resultData;

    FetchingDocumentCallback setfnc = [&resultData]( std::string&& jsondata )  {
        resultData.emplace_back( std::forward<std::string>(jsondata));
    };

    selectQuery( collname, query, setfnc );
    return resultData;
}

// Possible work with cursor
void ArangoDBCollectionAPI::selectAll( const std::string& collname,
                                       const QueryFields& queryFields,  FetchingDocumentIdCallback setfnc )
{
    std::unique_ptr<HttpMessage> request;
    if( queryFields.empty() ) {
        request = createSimpleAllRequest( collname );
    }
    else {
        std::string AQLquery = "FOR u IN ";
        AQLquery += collname;
        ArangoDBQuery query{ AQLquery, ArangoDBQuery::AQL };
        query.setQueryFields(queryFields);

        request = createAQLRequest( collname,  query  );
    }
    auto result = sendREQUEST(std::move(request));
    if( result->statusCode() >=  StatusBadRequest ) {
        return;
    }
    auto slice = result->slices().front();
    auto hasMore = slice.get("hasMore").getBool();
    auto query_result = slice.get("result");
    extractData( query_result, setfnc );

    // Read data from cursor
    while( hasMore ) {
        std::string id=slice.get("id").copyString();
        request = createREQUEST(RestVerb::Put, std::string("/_api/cursor/")+id);
        result = sendREQUEST(std::move(request));
        if( result->statusCode() >=  StatusBadRequest ) {
            return;  //need close cursor???
        }
        slice = result->slices().front();
        hasMore = slice.get("hasMore").getBool();
        query_result = slice.get("result");
        extractData( query_result, setfnc );
    }
}

std::vector<std::string> ArangoDBCollectionAPI::selectAll( const std::string& collname,
                                                           const QueryFields& queryFields )
{
    std::vector<std::string> resultData;

    FetchingDocumentIdCallback setfnc = [&resultData]( std::string&& jsondata, std::string&& ) {
        resultData.emplace_back( std::forward<std::string>(jsondata) );
    };
    selectAll( collname,  queryFields,  setfnc );
    return resultData;
}

// Fetches multiple documents by their keys
void ArangoDBCollectionAPI::lookupByKeys( const std::string& collname,
                                          const std::vector<std::string>& keys,  FetchingDocumentCallback setfnc )
{
    ::arangodb::velocypack::Builder builder;
    builder.openObject();
    builder.add("collection" , ::arangodb::velocypack::Value(collname) );
    builder.add("keys" ,  ::arangodb::velocypack::Value(::arangodb::velocypack::ValueType::Array) );
    for( const auto& key: keys ) {
        builder.add( ::arangodb::velocypack::Value(key));
    }
    builder.close();
    builder.close();

    auto request = createREQUEST(RestVerb::Put, std::string("/_api/simple/lookup-by-keys"));
    request->addVPack(builder.slice());
    //request->header.meta.erase("accept");

    auto result = sendREQUEST(std::move(request));
    if( result->statusCode() >=  StatusBadRequest ) {
        return;
    }
    auto slice = result->slices().front();
    auto query_result = slice.get("documents");
    extractData( query_result, setfnc );
}

std::vector<std::string> ArangoDBCollectionAPI::lookupByKeys( const std::string& collname,
                                                              const std::vector<std::string>& keys )
{
    std::vector<std::string> resultData;

    FetchingDocumentCallback setfnc = [&resultData]( std::string&& jsondata ) {
        resultData.emplace_back( std::forward<std::string>(jsondata) );
    };

    lookupByKeys( collname, keys, setfnc );
    return resultData;
}

void ArangoDBCollectionAPI::collectQuery( const std::string& collname,
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

    FetchingDocumentCallback setfnc = [&]( std::string&& jsondata) {
        if(jsondata != "null") {
            values.push_back(jsondata);
        }
        arango_logger->debug("FetchingDocumentCallback {}", jsondata);
    };
    ArangoDBQuery query{aqlQuery,ArangoDBQuery::AQL };
    selectQuery( collname, query,  setfnc );
}


void ArangoDBCollectionAPI::removeByTemplate(const std::string &collname, const std::string &jsontempl)
{
    try {
        auto data = ::arangodb::velocypack::Parser::fromJson(jsontempl, &parse_options);

        ::arangodb::velocypack::Builder builder;
        builder.openObject();
        builder.add("collection" , ::arangodb::velocypack::Value(collname) );
        builder.add("example" , data->slice() );
        builder.close();

        auto request = createREQUEST(RestVerb::Put, std::string("/_api/simple/remove-by-example"));
        request->addVPack(builder.slice());
        auto result =  sendREQUEST(std::move(request));
        auto slice1 = result->slices().front();

        if( result->statusCode() >=  StatusBadRequest ) {
            auto errmsg = slice1.get("errorMessage").copyString();
            arango_logger->error("Error removeByExampleRequest: {}", errmsg);
        }
        else {
            arango_logger->info(" {} - deleted : {}", collname, slice1.get("deleted").getInt());
        }
    }
    catch (::arangodb::velocypack::Exception& error )  {
        ARANGO_THROW( "ArangoDBCollectionAPI", 11, std::string("Velocypack error: ")+error.what());
    }
}


void ArangoDBCollectionAPI::removeEdges(const std::string& collname, const std::string& vertexkey )
{
    std::string vertexid= getId( collname, vertexkey );
    std::string to_templ = std::string("{\"_to\": \"")+ vertexid + "\" }";
    std::string from_templ = std::string("{\"_from\": \"")+ vertexid + "\" }";

    // get edges collections names
    auto edges = collectionNames( CollectionTypes::Edge );
    for( const auto& edgecoll: edges)  {
        removeByTemplate( edgecoll,  to_templ  );
        removeByTemplate( edgecoll,  from_templ  );
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

// Removes multiple documents by their keys
void ArangoDBCollectionAPI::removeByKeys( const std::string& collname,  const std::vector<std::string>& ids  )
{
    std::vector<std::string> keys;
    // ids to keys
    for( const auto& idit: ids ) {
        keys.push_back( getKey(  collname, idit ) );
    }
    ::arangodb::velocypack::Builder builder;
    builder.openObject();
    builder.add("collection" , ::arangodb::velocypack::Value(collname) );
    builder.add("keys" ,  ::arangodb::velocypack::Value(::arangodb::velocypack::ValueType::Array) );
    for( const auto& key: keys ) {
        builder.add( ::arangodb::velocypack::Value(key));
    }
    builder.close();
    builder.close();

    auto request = createREQUEST(RestVerb::Put, std::string("/_api/simple/remove-by-keys"));
    request->addVPack(builder.slice());
    auto result =  sendREQUEST(std::move(request));
    auto slice1 = result->slices().front();

    if( result->statusCode() >=  StatusBadRequest ) {
        auto errmsg = slice1.get("errorMessage").copyString();
        arango_logger->error("Error removeByKeys: {}", errmsg);
    }
    else {
        arango_logger->info(" {} - deleted : {} - ignored : {}", collname,
                            slice1.get("removed").getInt(), slice1.get("ignored").getInt() );
    }
}

} // namespace arangocpp
