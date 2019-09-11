#pragma once

#include <utility>
#include <string>
#include <map>
#include <functional>

namespace arangocpp {

/// Types of collection to select.
enum class CollectionTypes  {
    Vertex  = 0x0001,        ///< Document collection ( also referred to as vertex collections in the context of graphs)
    Edge    = 0x0002,        ///< Edge collections  ( used to create relations between documents )
    All = Vertex|Edge        ///< All Vertexes&Edges
};

inline bool operator&(const CollectionTypes& lhs, const CollectionTypes& rhs)
{ return static_cast<int>(lhs)&static_cast<int>(rhs); }

/// Callback function fetching document from a collection that match the specified condition
using  SetReadedFunction = std::function<void( const std::string& jsondata )>;
/// Callback function fetching document and _id from a collection that match the specified condition
using  SetReadedFunctionKey = std::function<void( const std::string& jsondata, const std::string& id )>;


/// Map of query fields used to generate return values for AQL.
/// <json name>-><jsonpath expression>
/// The following jsonpath expression could be used:    "name1.name2[3].name3"
using QueryFields = std::map<std::string, std::string >;

/// Map of query fields used to generate Filter values for AQL.
/// <jsonpath expression>-><value>
/// The following jsonpath expression could be used:    "name1.name2[3].name3"
using FieldValueMap = std::map<std::string, std::string >;

/// \class ArangoDBQuery can be used to retrieve data that are stored in ArangoDB.
class ArangoDBQuery
{

public:

    enum QueryType
    {
        Undef = -1,
        Template = 0 /* most of old queries*/,
        All = 1,
        EdgesFrom = 2,
        EdgesTo = 3,
        EdgesAll = 4,
        AQL = 6
    };


    /// Empty or All  query constructor.
    ArangoDBQuery( QueryType atype = All  ):
        query_type{atype}, find_condition{}, bind_vars{}, query_fields{}
    { }

    /// An AQL query text or json template constructor.
    ArangoDBQuery( const std::string& condition, QueryType atype );


    virtual ~ArangoDBQuery()
    {}

    /// Copy constructor
    ArangoDBQuery( const ArangoDBQuery& data) = default;
    /// Move constructor
    ArangoDBQuery( ArangoDBQuery&& data) = default;
    /// Copy assignment
    ArangoDBQuery &operator =( const ArangoDBQuery &other) = default;
    /// Move assignment
    ArangoDBQuery &operator =( ArangoDBQuery &&other)= default;


    /// Test for empty query.
    bool empty() const
    {
        return query_type == Undef;
    }

    /// Get query type.
    QueryType type() const
    {
        return query_type;
    }

    /// Get string with query ( an AQL query text or json template  ).
    const std::string& queryString() const
    {
        return find_condition;
    }

    /// Set json string with the bind parameter values need to be passed
    /// along with the query when it is executed.
    void  setBindVars( const std::string& jsonBindObject )
    {
        bind_vars = jsonBindObject;
    }

    /// Get the json string with bind values used in the query
    const std::string& bindVars() const
    {
        return bind_vars;
    }

    /// Set the fixed set of attributes from the collection is queried,
    /// then the query result values will have a homogeneous structure.
    void  setQueryFields( const QueryFields& mapFields )
    {
        query_fields = mapFields;
    }

    /// Get the fixed set of attributes from the collection is queried.
    const QueryFields& queryFields() const
    {
        return query_fields;
    }

    /// Generate AQL RETURN with predefined fields
    virtual std::string generateRETURN( bool isDistinct, const std::string& collvalue = "u" ) const;

    /// Generate AQL RETURN with mapFields fields
    static std::string generateRETURN( bool isDistinct, const QueryFields& mapFields,
                                       const std::string& collvalue = "u" );

    /// Generate AQL FILTER with predefined fields
    static std::string generateFILTER( const FieldValueMap& fldvalues, bool asTemplate = false,
                                       const std::string& collvalue = "u" );

    friend bool operator!=( const ArangoDBQuery&,  const ArangoDBQuery& );

protected:

    /// Query type.
    QueryType   query_type;
    /// String with query ( an AQL query text or json template  ).
    std::string find_condition;
    /// The bind parameter values need to be passed along with the query when it is executed.
    std::string bind_vars;
    /// List of fields, the fixed set of attributes from the collection.
    /// FOR u IN users
    /// RETURN { "id": u.id, "name": u.name }
    QueryFields query_fields;


};
extern ArangoDBQuery emptyQuery;


// https://www.arangodb.com/docs/stable/drivers/js-reference-collection.html

} // namespace arangocpp
