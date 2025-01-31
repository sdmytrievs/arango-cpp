#pragma once

#include "arango-cpp/arangoconnect.h"
#include "arango-cpp/arangoquery.h"

namespace arangocpp {

class RequestCurlObject;
class HttpMessage;
using StringMap = std::map<std::string, std::string>;

///  HTTP method ( CURLOPT_CUSTOMREQUEST)
enum class RestVerb
{
    Illegal = -1,
    Delete = 0,
    Get = 1,
    Post = 2,
    Put = 3,
    Head = 4,
    Patch = 5,
    Options = 6
};

class ArangoDBAPIBase
{

public:
    /// Check the document-handle example in to contain only
    /// characters officially allowed by ArangoDB.
    /// \return  a document-handle that contain only only allowed characters.
    static std::string sanitization( const std::string& documentHandle );

    ///  Constructor
    explicit ArangoDBAPIBase( const ArangoDBConnection& connectData );

    ///  Destructor
    virtual ~ArangoDBAPIBase()
    {}

    /// Reset connections to ArangoDB server.
    void resetDBConnection( const ArangoDBConnection& connectData );

    void updateBatchSize( int newsize )
    {
        batch_size = newsize;
    }
    int batchSize( ) const
    {
        return batch_size;
    }

    /// Test connection.
    bool testConnection(std::string& error_message);

    /// Notification of the result of the test connection request
    ///  when connecting to the ArangoDB server.
    std::string getConnectMessage() const {
      return connected_message;
    }

protected:

    /// ArangoDB connection data.
    ArangoDBConnection connect_data;
    /// Max number of records on transfer
    int batch_size = 500;
    /// Notification of the result of a test connection request
    std::string connected_message;

    virtual std::unique_ptr<HttpMessage> createREQUEST( RestVerb verb, std::string const& path,
                                                        StringMap const& parameter = StringMap() );
    virtual std::unique_ptr<HttpMessage> sendREQUEST( std::unique_ptr<HttpMessage> rq );

    // Genetrate _id from _key
    std::string getId( const std::string& collname, const std::string& key )
    {
        std::string rid = key;
        if( rid.find( collname+"/") == std::string::npos ) {
            rid = collname+"/"+key;
        }
        return rid;
    }

    // Genetrate _key from _id
    std::string getKey( const std::string& collname, const std::string& id )
    {
        std::string head = collname+"/";
        std::string rkey = id;

        auto pos  = rkey.find( head );
        if( pos != std::string::npos ) {
            rkey = rkey.substr( head.length() );
        }
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
    explicit ArangoDBCollectionAPI( const ArangoDBConnection& connectData ):
        ArangoDBAPIBase(connectData)
    {}

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
    /// Return the document-handle.
    std::string createDocument( const std::string& collname, const std::string& jsonrec );
    /// Read record by rid to JSON representation of a single document.
    bool readDocument( const std::string& collname, const std::string& documentHandle, std::string& jsonrec );
    /// Update existing document (a  JSON representation of a document update as an object).
    std::string updateDocument( const std::string& collname, const std::string& documentHandle, const std::string& jsonrec );
    /// Removes the document identified by document-handle.
    bool deleteDocument( const std::string& collname, const std::string& documentHandle );

    /// Gets an edge/vertex header from the given collection.
    /// \return  true is returned if the document was found.
    bool existsDocument( const std::string& collname, const std::string& documentHandle );

    // Selections

    /// Generate  query to outgoing or/and incoming edges ( query is a special format  EdgesX ).
    ArangoDBQuery queryEdgesToFrom( ArangoDBQuery::QueryType atype, const std::string& startVertex,
                                    const std::string& edgeCollections );

    /// Execute function to multiple documents by their keys.
    void lookupByKeys( const std::string& collname,  const std::vector<std::string>& keys,  FetchingDocumentCallback setfnc );
    /// Get list of json strings with multiple documents by their keys.
    std::vector<std::string> lookupByKeys(const std::string &collname, const std::vector<std::string> &keys);

    /// Execute function to selected records by condition ( query is simple, query by-example or AQL ).
    void selectQuery( const std::string& collname,  const ArangoDBQuery& query,  FetchingDocumentCallback setfnc );
    /// Get list of json strings with documents selected by condition ( query is simple, query by-example or AQL ).
    std::vector<std::string> selectQuery(const std::string &collname, const ArangoDBQuery &query);

    /// Execute function to all records into collection, if isComlexFields false  extracts only Query Fields.
    void selectAll( const std::string& collname, const QueryFields& queryFields, FetchingDocumentIdCallback setfnc );
    /// Get list of json strings with all documents into collection, extracts only queryFields.
    std::vector<std::string> selectAll(const std::string &collname, const QueryFields &queryFields);

    /// Collect distinct values for field fpath into collection collname.
    void collectQuery( const std::string& collname, const std::string& fpath, std::vector<std::string>& values );

    // Multi Records deletion  (AQL remove ?)

    /// Removes multiple documents by their keys.
    void removeByKeys( const std::string& collname,  const std::vector<std::string>& keys  );

    /// Remove records from collection collname using template.
    void removeByTemplate( const std::string& collname,  const std::string& jsontempl  );

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

};


} // namespace arangocpp
