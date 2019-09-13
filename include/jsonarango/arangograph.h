#pragma once

#include "arangocollection.h"

namespace arangocpp {

/// \class  ArangoDBGraphAPI implement the API for manipulating graphs.
class ArangoDBGraphAPI : public ArangoDBAPIBase
{

public:

    ///  Constructor
    explicit ArangoDBGraphAPI( const ArangoDBConnect& connectData ):
        ArangoDBAPIBase(connectData)
    { }

    ///  Destructor
    ~ArangoDBGraphAPI();

    // Graph commands

    /// Checks whether the graph exists.
    bool existGraph(const std::string& graphname );

    /// Lists all graph names stored in this database.
    std::set<std::string> graphNames();

    /// Creates a graph with the given properties for this graph’s name.
    /// The creation of a graph requires the name of the graph and a definition of its edges.
    void createGraph(const std::string& graphname, const std::string& edgeDefinitions);
    /// Delete an existing graph.
    void removeGraph( const std::string& graphname );

    /// Fetches all (edges/vertexes) collections from the graph.
    std::set<std::string> graphCollectionNames( const std::string& graphname, const std::string& colltype );

    /// Adds a vertex collection to the set of collections of the graph.
    /// If the collection does not exist, it will be created.
    void addVertexGraph(const std::string& graphname, const std::string& collname );

    /// Adds an additional edge definition to the graph.
    void addEdgeGraph(const std::string& graphname, const std::string& edgeDefinition);


    // These functions implement the API for manipulating documents (CRUD)in an existing graph.

    /// Creates a new vertex/edge document with the given data in an existing graph.
    std::string createRecord( const std::string& graphname, const std::string& colltype,
                              const std::string& collname, const std::string& jsonrec );

    /// Retrieves the vertex/edge document with the given documentHandle from the collection.
    bool readRecord( const std::string& graphname, const std::string& colltype,
                     const std::string& collname, const std::string& documentHandle, std::string& jsonrec );

    /// Replaces the data of an edge/vertex in the collection
    std::string updateRecord( const std::string& graphname, const std::string& colltype,
                              const std::string& collname, const std::string& documentHandle, const std::string& jsonrec );

    /// Deletes the vertex/edge with the given documentHandle from the collection.
    bool deleteRecord( const std::string& graphname, const std::string& colltype,
                       const std::string& collname, const std::string& documentHandle );

};


} // namespace arangocpp
