#ifndef ARANGOGRAPH_H
#define ARANGOGRAPH_H

#include "arangocollection.h"

namespace jsonio { namespace arangodb {


class ArangoDBGraphAPI : public ArangoDBAPIBase
{

public:

    ///  Constructor
    explicit ArangoDBGraphAPI( const jsonio::ArangoDBConnect& connectData ):
        ArangoDBAPIBase(connectData)
    { }

    ///  Destructor
    ~ArangoDBGraphAPI();

    // Graph commands

    /// Get a graph from the graph module
    bool ExistGraph(const std::string& graphname );

    /// The creation of a graph requires the name of the graph and a definition of its edges
    void CreateGraph(const std::string& graphname, const std::string& edgeDefinitions);

    /// Lists all edge/vertex collections used in this graph
    std::set<std::string> getGraphCollectionNames( const std::string& graphname, const std::string& colltype );

    /// Adds a vertex collection to the set of collections of the graph.
    /// If the collection does not exist, it will be created.
    void AddVertexGraph(const std::string& graphname, const std::string& collname );

    /// Adds an additional edge definition to the graph.
    void AddEdgeGraph(const std::string& graphname, const std::string& edgeDefinition);


    // work with documents

    /// Creates an edge/vertex in an existing graph
    std::string CreateRecord( const std::string& graphname, const std::string& colltype,
      const std::string& collname, const std::string& jsonrec );

    /// Gets an edge/vertex from the given collection to json std::string
    bool ReadRecord( const std::string& graphname, const std::string& colltype,
      const std::string& collname, const std::string& rid, std::string& jsonrec );

    /// Replaces the data of an edge/vertex in the collection
    std::string UpdateRecord( const std::string& graphname, const std::string& colltype,
      const std::string& collname, const std::string& rid, const std::string& jsonrec );

    /// Removes an edge/vertex from the collection
    bool DeleteRecord( const std::string& graphname, const std::string& colltype,
      const std::string& collname, const std::string& rid );

};


}  }

#endif // ARANGOGRAPH_H
