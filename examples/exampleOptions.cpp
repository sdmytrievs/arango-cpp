#include <iostream>
#include "jsonarango/arangocollection.h"
#include "jsonarango/arangoexception.h"
#include <velocypack/Collection.h>

const std::string aql_fun_substances_reactions_ids = "/* Get substances */ \n"
                                                     "LET substances_ = ( \n"
                                                     "/* Get substances that are defined through a reaction */ \n"
                                                     "LET reacdcs_ = ( \n"
                                                     "    FOR s IN substances \n"
                                                     "        FILTER s._id IN @ids \n"
                                                     "        FOR r IN 1..1 INBOUND s defines \n"
                                                     "        RETURN { \n"
                                                     "            name:   s.properties.name, \n"
                                                     "            symbol: s.properties.symbol, \n"
                                                     "            reaction : r.properties.symbol, \n"
                                                     "            formula:    s.properties.formula, \n"
                                                     "            formula_charge: s.properties.formula_charge, \n"
                                                     "            mass_per_mole:  s.properties.mass_per_mole, \n"
                                                     "            aggregate_state:    s.properties.aggregate_state, \n"
                                                     "            class_:    s.properties.class_, \n"
                                                     "            limitsTP:   s.properties.limitsTP, \n"
                                                     "            Tst:    s.properties.Tst, \n"
                                                     "            Pst:    s.properties.Pst, \n"
                                                     "            TPMethods:  s.properties.TPMethods, \n"
                                                     "            sm_heat_capacity_p: s.properties.sm_heat_capacity_p, \n"
                                                     "            sm_gibbs_energy:    s.properties.sm_gibbs_energy, \n"
                                                     "            sm_enthalpy:    s.properties.sm_enthalpy, \n"
                                                     "            sm_entropy_abs: s.properties.sm_entropy_abs, \n"
                                                     "            sm_volume:  s.properties.sm_volume, \n"
                                                     "            m_compressibility: s.properties.m_compressibility, \n"
                                                     "            m_expansivity: s.properties.m_expansivity, \n"
                                                     "            id: s._id \n"
                                                     "        } \n"
                                                     ") \n"
                                                     "/* Get substances from list of records ids */ \n"
                                                     "LET selected_substances_ = ( \n"
                                                     "    FOR s IN substances \n"
                                                     "        FILTER s._id IN @ids \n"
                                                     "        FILTER s.properties.symbol NOT IN reacdcs_[*].symbol \n"
                                                     "        RETURN { \n"
                                                     "            name:   s.properties.name, \n"
                                                     "            symbol: s.properties.symbol, \n"
                                                     "            formula:    s.properties.formula, \n"
                                                     "            formula_charge: s.properties.formula_charge, \n"
                                                     "            mass_per_mole:  s.properties.mass_per_mole, \n"
                                                     "            aggregate_state:    s.properties.aggregate_state, \n"
                                                     "            class_:    s.properties.class_, \n"
                                                     "            limitsTP:   s.properties.limitsTP, \n"
                                                     "            Tst:    s.properties.Tst, \n"
                                                     "            Pst:    s.properties.Pst, \n"
                                                     "            TPMethods:  s.properties.TPMethods, \n"
                                                     "            sm_heat_capacity_p: s.properties.sm_heat_capacity_p, \n"
                                                     "            sm_gibbs_energy:    s.properties.sm_gibbs_energy, \n"
                                                     "            sm_enthalpy:    s.properties.sm_enthalpy, \n"
                                                     "            sm_entropy_abs: s.properties.sm_entropy_abs, \n"
                                                     "            sm_volume:  s.properties.sm_volume, \n"
                                                     "            m_compressibility: s.properties.m_compressibility, \n"
                                                     "            m_expansivity: s.properties.m_expansivity, \n"
                                                     "            id: s._id \n"
                                                     "        }) \n"
                                                     "RETURN APPEND(reacdcs_, selected_substances_, true)  \n"
                                                     ") \n"
                                                     "/* Get reactions */ \n"
                                                     "LET reactions_ = ( \n"
                                                     "LET reactions_defines_ = ( \n"
                                                     "    FOR s IN substances \n"
                                                     "        FILTER s._id IN @ids  \n"
                                                     "        FOR r IN 1..1 INBOUND s defines \n"
                                                     "        LET reactants_ = ( \n"
                                                     "            FOR ss, t IN 1..1 INBOUND r takes \n"
                                                     "/*            LET ids_2 = PUSH(ids_, ss._id, true) /* add substance id to idlist */ \n"
                                                     "            RETURN { \n"
                                                     "                symbol: ss.properties.symbol, \n"
                                                     "                coefficient: t.properties.stoi_coeff, \n"
                                                     "                id: ss._id \n"
                                                     "            } \n"
                                                     "        ) \n"
                                                     "        RETURN { \n"
                                                     "            name:   r.properties.name, \n"
                                                     "            symbol: r.properties.symbol, \n"
                                                     "            substance : s.properties.symbol, \n"
                                                     "            equation:    r.properties.equation, \n"
                                                     "            reactants: reactants_, \n"
                                                     "            limitsTP:   r.properties.limitsTP, \n"
                                                     "            Tst:    r.properties.Tst, \n"
                                                     "            Pst:    r.properties.Pst, \n"
                                                     "            TPMethods:  r.properties.TPMethods, \n"
                                                     "            logKr : r.properties.logKr, \n"
                                                     "            drsm_heat_capacity_p : r.properties.drsm_heat_capacity_p, \n"
                                                     "            drsm_gibbs_energy : r.properties.drsm_gibbs_energy, \n"
                                                     "            drsm_enthalpy : r.properties.drsm_enthalpy, \n"
                                                     "            drsm_entropy : r.properties.drsm_entropy, \n"
                                                     "            drsm_volume : r.properties.drsm_volume, \n"
                                                     "            id: r._id \n"
                                                     "        } \n"
                                                     ") \n"
                                                     "LET selected_reactions_ = ( \n"
                                                     "    FOR r IN reactions \n"
                                                     "        FILTER r._id IN @ids \n"
                                                     "        FILTER r.properties.symbol NOT IN reactions_defines_[*].symbol \n"
                                                     "        LET reactants_ = ( \n"
                                                     "            FOR ss, t IN 1..1 INBOUND r takes \n"
                                                     "            RETURN { \n"
                                                     "                symbol: ss.properties.symbol, \n"
                                                     "                coefficient: t.properties.stoi_coeff, \n"
                                                     "                id: ss._id \n"
                                                     "            } \n"
                                                     "        ) \n"
                                                     "        RETURN { \n"
                                                     "            name:   r.properties.name, \n"
                                                     "            symbol: r.properties.symbol, \n"
                                                     "            equation:    r.properties.equation, \n"
                                                     "            reactants: reactants_, \n"
                                                     "            limitsTP:   r.properties.limitsTP, \n"
                                                     "            Tst:    r.properties.Tst, \n"
                                                     "            Pst:    r.properties.Pst, \n"
                                                     "            TPMethods:  r.properties.TPMethods, \n"
                                                     "            logKr : r.properties.logKr, \n"
                                                     "            drsm_heat_capacity_p : r.properties.drsm_heat_capacity_p, \n"
                                                     "            drsm_gibbs_energy : r.properties.drsm_gibbs_energy, \n"
                                                     "            drsm_enthalpy : r.properties.drsm_enthalpy, \n"
                                                     "            drsm_entropy : r.properties.drsm_entropy, \n"
                                                     "            drsm_volume : r.properties.drsm_volume, \n"
                                                     "            id: r._id \n"
                                                     "        }) \n"
                                                     "RETURN APPEND(reactions_defines_, selected_reactions_, true)  \n"
                                                     ") \n"
                                                     "/* get ids of all traversed data */ \n"
                                                     "LET ids_substances_ = (substances_[*][*].id) \n"
                                                     "LET ids_reactions_ = (reactions_[*][*].id) \n"
                                                     "LET ids_reactants_ = ( \n"
                                                     "    FOR r IN reactions_ \n"
                                                     "            RETURN r[*].reactants[*].id \n"
                                                     ") \n"
                                                     "LET ids_ =  (APPEND(FLATTEN(ids_substances_,1), FLATTEN(ids_reactions_,1), true)) \n"
                                                     "LET ids_all =  SORTED_UNIQUE((FLATTEN(APPEND(ids_, FLATTEN(ids_reactants_,1), true),1))) \n"
                                                     "RETURN {\"substances\": SORTED_UNIQUE(FLATTEN(substances_,1)) , \"reactions\": SORTED_UNIQUE(FLATTEN(reactions_,1)), \"ids\": ids_all} \n";



void printData( const std::string&  title, const std::vector<std::string>& values )
{
    std::cout <<  title <<  std::endl;
    for( const auto& jsondata: values)
        std::cout <<  jsondata <<  std::endl;
    std::cout <<  std::endl;
}

int main(int, char* [])
{
    std::string settingsFileName = "examples-cfg.json";
    // Test collection name
    std::string collectionName = "elements";

     // Record keys
    std::vector<std::string> recKeys;
    std::vector<std::string> recjsonValues;

    try{
        std::cout << "Start " << std::endl;

        // Get Arangodb connection data( load settings from "examples-cfg.json" config file )
        arangocpp::ArangoDBConnection data = arangocpp::connectFromConfig( "examples-cfg.json" );
        // Create database connection
        arangocpp::ArangoDBCollectionAPI connect{data};

        // Define call back function
        arangocpp::FetchingDocumentCallback setfnc = [&recjsonValues]( const std::string& jsondata )
        {
            recjsonValues.push_back(jsondata);
        };

        std::string options = "{ \"maxPlans\" : 1, "
                              "  \"optimizer\" : { \"rules\" : [ \"-all\", \"+remove-unnecessary-filters\" ]  } } ";
        arangocpp::ArangoDBQuery    aqlquery( aql_fun_substances_reactions_ids, arangocpp::ArangoDBQuery::AQL );
        aqlquery.setBindVars("{ \"ids\": [\"reactions/ZrF2(OH)2@;18:MINES;0\",\"substances/(NpO2)2(OH)2+2;2:SC_AQSOLUTE;15:PSI_NAGRA\"] }");
        aqlquery.setOptions(options);
        connect.selectQuery( collectionName, aqlquery, setfnc );
        printData( "Select records by AQL query", recjsonValues );

        std::cout << "End " << std::endl;

    }
    catch(arangocpp::arango_exception& e)
    {
        std::cout << e.header() << e.what() <<  std::endl;
    }
    catch(std::exception& e)
    {
        std::cout <<  " std::exception" << e.what() <<  std::endl;
    }
    catch(...)
    {
        std::cout <<  " unknown exception" <<  std::endl;
    }

    return 0;
}

