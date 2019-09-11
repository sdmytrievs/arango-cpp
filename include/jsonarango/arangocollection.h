#pragma once

#include <set>
#include "arangorequests.h"
#include "arangoconnect.h"
#include "query.h"

namespace arangocpp {

#define  DEBUG_OUTPUT( message, data ) if (0) { std::cout << message << to_string(*(data)) << std::endl; }
//#define  DEBUG_OUTPUT( message, data )  { std::cout << message << to_string(*(data)) << std::endl; }
#define JSONIO_LOG   if (0) std::cout
//#define JSONIO_LOG    std::cout

class ArangoDBAPIBase
{


public:

    ///  Constructor
    explicit ArangoDBAPIBase( const ArangoDBConnect& connectData )
    {
        dump_options.unsupportedTypeBehavior = ::arangodb::velocypack::Options::NullifyUnsupportedType;
        parse_options.validateUtf8Strings = true;
        resetDBConnection(connectData);
    }

    ///  Destructor
    virtual ~ArangoDBAPIBase()
    { }

    /// Reset connections to ArangoDB server.
    void resetDBConnection( const ArangoDBConnect& connectData );

    void updateBatchSize( int newsize )
    {
        batch_size = newsize;
    }
    int batchSize( ) const
    {
        return batch_size;
    }

    /// Test connection.
    void testConnection();

protected:

    /// ArangoDB connection data.
    ArangoDBConnect connect_data;
    /// Max number of records on transfer
    int batch_size = 500;
    ::arangodb::velocypack::Options dump_options;
    ::arangodb::velocypack::Options parse_options;

    virtual std::unique_ptr<HttpMessage> createREQUEST( RestVerb verb, std::string const& path,
                                                        StringMap const& parameter = StringMap() );
    virtual std::unique_ptr<HttpMessage> sendREQUEST( std::unique_ptr<HttpMessage> rq );

    // Genetrate _id from _key
    std::string getId( const std::string& collname, const std::string& key )
    {
        std::string rid = key;
        if( rid.find( collname+"/") == std::string::npos )
            rid = collname+"/"+key;
        return rid;
    }

    // Genetrate _key from _id
    std::string getKey( const std::string& collname, const std::string& id )
    {
        std::string head = collname+"/";
        std::string rkey = id;

        auto pos  = rkey.find( head );
        if( pos != std::string::npos )
            rkey = rkey.substr( head.length() );
        return rkey;
    }

    /// Copy constructor
    ArangoDBAPIBase( const ArangoDBAPIBase& data) = default;
    /// Move constructor
    ArangoDBAPIBase( ArangoDBAPIBase&& data) = default;
    /// Copy assignment
    ArangoDBAPIBase &operator =( const ArangoDBAPIBase &other) = default;
    /// Move assignment
    ArangoDBAPIBase &operator =( ArangoDBAPIBase &&other)= default;

};

/// \class  ArangoDBCollectionAPI the API for manipulating collections and documents into.
/// The Collection API is implemented by all Collection instances,
/// regardless of their specific type.
class ArangoDBCollectionAPI : public ArangoDBAPIBase
{

public:

    ///  Constructor
    explicit ArangoDBCollectionAPI( const ArangoDBConnect& connectData ):
        ArangoDBAPIBase(connectData)
    { }

    ~ArangoDBCollectionAPI();

    // These functions implement the API for modifying collections.

    /// Test exist collection.
    bool existCollection(const std::string& collname );
    /// Create collection if no exist.
    /// \param ctype must be "edge" for edge collections.
    void createCollection(const std::string& collname, const std::string& ctype);
    /// Drops the collection identified by collection-name.
    void dropCollection(const std::string& collname );
    /// Collect collection names in current database ( or only Edges/Vertex collections ).
    std::set<std::string> collectionNames( CollectionTypes ctype );

    // These functions implement the API for manipulating documents (CRUD)

    /// Create new record from a JSON representation of a single document.
    /// Return a document-handle.
    std::string createRecord( const std::string& collname, const std::string& jsonrec );
    /// Read record by rid to JSON representation of a single document.
    bool readRecord( const std::string& collname, const std::string& documentHandle, std::string& jsonrec );
    /// Update existing document (a  JSON representation of a document update as an object).
    std::string updateRecord( const std::string& collname, const std::string& documentHandle, const std::string& jsonrec );
    /// Removes the document identified by document-handle.
    bool deleteRecord( const std::string& collname, const std::string& documentHandle );

    /// Gets an edge/vertex header from the given collection.
    /// \return  true is returned if the document was found.
    bool existsRecord( const std::string& collname, const std::string& documentHandle );

    // Selections

    /// Generate  query to outgoing or/and incoming edges ( query is a special format  EdgesX ).
    ArangoDBQuery queryEdgesToFrom( ArangoDBQuery::QueryType atype, const std::string& startVertex,
                                    const std::string& edgeCollections );

    /// Execute function to multiple documents by their keys.
    void lookupByKeys( const std::string& collname,  const std::vector<std::string>& keys,  SetReadedFunction setfnc );

    /// Execute function to selected records by condition ( query is simple, query by-example or AQL ).
    void selectQuery( const std::string& collname,  const ArangoDBQuery& query,  SetReadedFunction setfnc );

    /// Execute function to all records into collection, if isComlexFields false  extracts only Query Fields.
    void selectAll( const std::string& collname, const QueryFields& queryFields, SetReadedFunctionKey setfnc );

    /// Collect distinct values for field fpath into collection collname.
    void collectQuery( const std::string& collname, const std::string& fpath, std::vector<std::string>& values );

    // Multi Records deletion  (AQL remove ?)

    /// Removes multiple documents by their keys.
    void removeByKeys( const std::string& collname,  const std::vector<std::string>& keys  );

    /// Remove records from collection collname using template.
    void removeByExample( const std::string& collname,  const std::string& jsontempl  );

    /// Remove outgoing and incoming edges for vertex.
    void removeEdges( const std::string& collname, const std::string& vertexid );

protected:

    /// Generate request to returns all documents of a collection
    std::unique_ptr<HttpMessage> createSimpleAllRequest( const std::string& collname );

    /// Generate simple query by-example to return all documents of a collection matching a given example
    std::unique_ptr<HttpMessage> createByExampleRequest( const std::string& collname,  const std::string& jsontempl  );

    /// Generate request to AQL query cursor
    std::unique_ptr<HttpMessage> createAQLRequest( const std::string& collname,  const ArangoDBQuery& query  );

    /// Generate request to Edges query cursor
    std::unique_ptr<HttpMessage> createEdgeRequest( const std::string& collname,  const ArangoDBQuery& query  );

    /// Execute user function to all records data
    void extractData( const ::arangodb::velocypack::Slice& sresult,  SetReadedFunction setfnc );
    /// Execute user function to all records data
    void extractData( const ::arangodb::velocypack::Slice& sresult,  SetReadedFunctionKey setfnc );

};


} // namespace arangocpp
