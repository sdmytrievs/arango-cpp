#ifndef ARANGOCOLLECTION_H
#define ARANGOCOLLECTION_H

#include <set>
//#include "jsonio/dbarango.h"
#include "connection_data.h"
#include "arangorequests.h"

namespace jsonio { namespace arangodb {

#define  DEBUG_OUTPUT( message, data ) if (0) { std::cout << message << arangodb::to_string(*(data)) << std::endl; }
//#define  DEBUG_OUTPUT( message, data )  { std::cout << message << arangodb::to_string(*(data)) << std::endl; }
#define JSONIO_LOG   if (0) std::cout
//#define JSONIO_LOG    std::cout

class ArangoDBAPIBase
{

 protected:

    /// ArangoDB connection data
    jsonio::ArangoDBConnect _connect_data;
    /// Max number of records on transfer
    int _batchSize = 500;
    ::arangodb::velocypack::Options _dumpoptions;
    ::arangodb::velocypack::Options _parseoptions;

    virtual std::unique_ptr<arangodb::HttpMessage> createRequest( arangodb::RestVerb verb,
        std::string const& path, arangodb::StringMap const& parameter = arangodb::StringMap() );
    virtual std::unique_ptr<arangodb::HttpMessage> sendRequest(std::unique_ptr<arangodb::HttpMessage> rq );

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


public:

    ///  Constructor
    explicit ArangoDBAPIBase( const jsonio::ArangoDBConnect& connectData )
    {
        _dumpoptions.unsupportedTypeBehavior = ::arangodb::velocypack::Options::NullifyUnsupportedType;
        _parseoptions.validateUtf8Strings = true;
        resetDBConnection(connectData);
    }

    ///  Destructor
    virtual ~ArangoDBAPIBase()
    { }

    /// Reset connections to ArangoDB server
    void resetDBConnection( const jsonio::ArangoDBConnect& connectData );

    void updateBatchSize( int newsize )
    {
      _batchSize = newsize;
    }
    int getBatchSize( ) const
    {
      return _batchSize;
    }

    /// Test connection
    void testConnection();

 };


class ArangoDBCollectionAPI : public ArangoDBAPIBase
{
    /// Generate request to returns all documents of a collection
    std::unique_ptr<arangodb::HttpMessage> createSimpleAllRequest( const std::string& collname );

    /// Generate simple query by-example to return all documents of a collection matching a given example
    std::unique_ptr<arangodb::HttpMessage> createByExampleRequest
         ( const std::string& collname,  const std::string& jsontempl  );

    /// Generate request to AQL query cursor
    std::unique_ptr<arangodb::HttpMessage> createAQLRequest
         ( const std::string& collname,  const DBQueryData& query  );

    /// Execute user function to all records data
    void extractData( const ::arangodb::velocypack::Slice& sresult,  jsonio::SetReadedFunction setfnc );
    /// Execute user function to all records data
    void extractData( const ::arangodb::velocypack::Slice& sresult,  jsonio::SetReadedFunctionKey setfnc );

 public:

    ///  Constructor
    explicit ArangoDBCollectionAPI( const jsonio::ArangoDBConnect& connectData ):
        ArangoDBAPIBase(connectData)
    { }

    ///  Destructor
    ~ArangoDBCollectionAPI();

    // collection create functions

    /// Test exist collection
    bool ExistCollection(const std::string& collname );
    /// Create collection if no exist
    /// \param ctype must be "edge" for edge collections
    void CreateCollection(const std::string& collname, const std::string& ctype);
    /// Drops the collection identified by collection-name
    void DropCollection(const std::string& collname );
    /// Collect collection names in current database ( or only Edges/Vertex collections )
    std::set<std::string> getCollectionNames( jsonio::TAbstractDBDriver::CollectionTypes ctype );

    // CRUD documents

    /// Create new record from a JSON representation of a single document
    std::string CreateRecord( const std::string& collname, const std::string& jsonrec );
    /// Read record by rid to JSON representation of a single document
    bool ReadRecord( const std::string& collname, const std::string& rid, std::string& jsonrec );
    /// Update existing document (a  JSON representation of a document update as an object)
    std::string UpdateRecord( const std::string& collname, const std::string& rid, const std::string& jsonrec );
    /// Removes the document identified by document-handle
    bool DeleteRecord( const std::string& collname, const std::string& rid );

    /// Gets an edge/vertex header from the given collection
    /// \return  true is returned if the document was found
    bool ExistsRecord( const std::string& collname, const std::string& rid );

    // Selections

    /// Execute function to multiple documents by their keys
    void LookupByKeys( const std::string& collname,  const std::vector<std::string>& keys,  SetReadedFunction setfnc );

    /// Execute function to selected records by condition ( query is simple, query by-example or AQL )
    void SelectQuery( const std::string& collname,  const DBQueryData& query,  SetReadedFunction setfnc );

    /// Execute function to outgoing or/and incoming edges ( query is a special format  qEdgesX   )
    void SearchEdgesToFrom(const std::string& collname, const DBQueryData& query,  SetReadedFunction setfnc);

    /// Execute function to outgoing or incoming edges ( query is query by-example - old EJDB format )
    void SearchEdgesToFromOld(const std::string& collname, const DBQueryData& query,  SetReadedFunction setfnc);

    /// Execute function to all records into collection, if isComlexFields false  extracts only Query Fields
    void AllQueryFields( bool isComlexFields, const std::string& collname,
              const std::set<std::string>& queryFields, jsonio::SetReadedFunctionKey setfnc );

    /// Collect distinct values for field fpath into collection collname
    void CollectQuery( const std::string& collname, const std::string& fpath, std::vector<std::string>& values );

    // Multi Records deletion

    /// Removes multiple documents by their keys
    void RemoveByKeys( const std::string& collname,  const std::vector<std::string>& keys  );

    /// Remove records from collection collname using template
    void RemoveByExample( const std::string& collname,  const std::string& jsontempl  );

    /// Remove outgoing and incoming edges for vertex
    void RemoveEdges( const std::string& collname, const std::string& vertexid );

};


}  }

#endif // ARANGOCOLLECTION_H
