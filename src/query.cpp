#include "query.h"

namespace arangocpp {


ArangoDBQuery emptyQuery(ArangoDBQuery::Undef);

bool operator !=( const ArangoDBQuery& iEl,  const ArangoDBQuery& iEr)
{
    return (iEl.query_type != iEr.query_type) || (iEl.find_condition != iEr.find_condition);
}

ArangoDBQuery::ArangoDBQuery( const std::string &condition, ArangoDBQuery::QueryType atype):
    query_type{atype}, find_condition{condition}, bind_vars{}, query_fields{}
{
    if( find_condition.empty() )
        query_type = All;
    //else
    //    if( query_type == Template )
    //        find_condition = replace_all( find_condition, "\'", "\"");  // not all only
}

std::string ArangoDBQuery::generateRETURN( bool isDistinct, const std::string &collvalue ) const
{
    return  generateRETURN( isDistinct, query_fields, collvalue );
}

//RETURN { "_label": u._label, "properties.symbol": u.properties.symbol, "properties.sourcetdb":u.properties.sourcetdb,
// "properties.name": u.properties.name, "properties.formula":u.properties.formula }
std::string ArangoDBQuery::generateRETURN( bool isDistinct, const QueryFields& mapFields, const std::string& collvalue )
{
    std::string retdata{"\nRETURN "};
    if( isDistinct )
        retdata += "DISTINCT ";
    if( mapFields.empty() )
        retdata += collvalue + " ";
    else
    {
        retdata += " { ";
        std::string fldslst{""};
        for( auto fld: mapFields )
        {
            if( !fldslst.empty() )
                fldslst += ", ";
            fldslst += " \"" + fld.first + "\" : " + collvalue + "." + fld.second;
        }
        retdata += fldslst+ " } ";
    }
    return   retdata;
}

// For string values must be into (" or ')
std::string ArangoDBQuery::generateFILTER(const FieldValueMap &fldvalues, bool asTemplate, const std::string &collvalue)
{
    std::string generated;
    if( asTemplate )
    {
        for(auto& ent : fldvalues)
        {
            if( !generated.empty() )
                generated += ", ";
            generated += " \"" + ent.first +"\" : " + ent.second+ " ";
        }
    }else
    {
        for(auto& ent : fldvalues)
        {
            if( !generated.empty() )
                generated += " && ";
            generated += collvalue + "." + ent.first + " == " + ent.second;
        }
        generated = "\nFILTER " + generated +" ";
    }

    return generated;
}


} // namespace arangocpp
