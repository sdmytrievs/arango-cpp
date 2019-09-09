#pragma once

#include <utility>
#include <string>
#include <map>

namespace arango {

/// Types of collection to select.
enum class CollectionTypes {
    Vertex  = 0x0001,        ///< Document collection ( also referred to as vertex collections in the context of graphs)
    Edge    = 0x0002,        ///< Edge collections  ( used to create relations between documents )
    All = Vertex|Edge        ///< All Vertexes&Edges
};

/// Map of query fields used to generate return values for AQL.
/// <json name>-><field path into json record>
using QueryFields = std::map<std::string, std::string >;


/// \class AQLQuery can be used to retrieve data that are stored in ArangoDB.
class AQLQuery
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


  /// Query type.
  QueryType   query_type;
  /// String with query ( an AQL query text or json template  ).
  std::string find_condition;
  /// The bind parameter values need to be passed along with the query when it is executed.
  std::string bind_vars;
  /// List of fields, if extract part of record in AQL.
  QueryFields query_fields;


  /// Empty or All  query constructor.
  AQLQuery( QueryType atype = All  ):
      query_type{atype}, find_condition{}, bind_vars{}, query_fields{}
  { }

  /// String constructor
  AQLQuery( std::string&& condition, QueryType atype ):
      query_type{atype}, find_condition{std::forward<std::string>(condition)}, bind_vars{}, query_fields{}
  {
    if( find_condition.empty() )
       query_type = All;
    else
       if( query_type == Template )
         find_condition = replace_all( find_condition, "\'", "\"");
  }

  AQLQuery( const AQLQuery& data) = default;
  AQLQuery( AQLQuery&& data) = default;

  virtual ~AQLQuery()
  {}

  bool empty() const
  {
    return query_type == Undef;
  }

  int type() const
  {
    return query_type;
  }

  const std::string& queryString() const
   {
         return find_condition;
   }

  void  setBindVars( const std::string& jsonBindObject )
  {
         bind_vars = jsonBindObject;
  }

  const std::string& bindVars() const
  {
         return bind_vars;
  }

  void  setQueryFields( const QueryFields& mapFields )
  {
        _queryFields = mapFields;
  }

  template <typename Container>
  void  setQueryFields( const Container& listFields )
  {
      _queryFields.clear();

      typename Container::const_iterator itr = listFields.begin();
      typename Container::const_iterator end = listFields.end();
      for (; itr != end; ++itr)
      {
          std::string fld = *itr;
          replaceall( fld, '.', '_');
          _queryFields[fld] = *itr;
      }

  }

  const QueryFields& getQueryFields() const
  {
         return _queryFields;
  }

  /// Generate AQL RETURN with predefined fields
  static std::string generateReturn( bool isDistinct, const QueryFields& mapFields, const std::string& collvalue = "u" );
  /// Generate AQL RETURN with predefined fields
  std::string generateReturn(const std::string& collvalue = "u" ) const
  {
     return generateReturn( false, _queryFields, collvalue );
  }
  /// Generate AQL FILTER with predefined fields
  static std::string generateFILTER(  const jsonio::FieldSetMap& fldvalues, bool asTemplate = false,


                                      const std::string& collvalue = "u" );

  friend bool operator!=( const AQLQuery&,  const AQLQuery& );

protected:

  /// Query type.
  QueryType   query_type;
  /// String with query ( an AQL query text or json template  ).
  std::string find_condition;
  /// The bind parameter values need to be passed along with the query when it is executed.
  std::string bind_vars;
  /// List of fields, if extract part of record in AQL.
  QueryFields query_fields;


};
extern AQLQuery emptyQuery;


// https://www.arangodb.com/docs/stable/drivers/js-reference-collection.html

} // namespace arango
