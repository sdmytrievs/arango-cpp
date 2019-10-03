/// Test example for class  ArangoDBGraphAPI implement the API for manipulating graphs..

#include <iostream>
#include "jsonarango/arangograph.h"
#include "jsonarango/arangoexception.h"

void printData( const std::string&  title, const std::set<std::string>& values )
{
    std::cout <<  title ;
    for( const auto& jsondata: values)
        std::cout <<  jsondata <<  "  ";
    std::cout <<  std::endl;
}


int main(int, char* [])
{
    std::string settingsFileName = "examples-cfg.json";

    // Test collection names
    std::string vertex1 = "startVertex";
    std::string vertex2 = "endVertex";
    std::string edge12 = "linkEdge";
    std::string edgeDefinition = "[ {  \"collection\" : \""+ edge12+ "\","
                                 "     \"from\" : [ \"" + vertex1 + "\" ],"
                                 "     \"to\" :   [ \"" + vertex2 + "\"  ] } ] ";


    // graph name
    std::string graph = "links";


    try{

        // Get Arangodb connection data( load settings from "examples-cfg.json" config file )
        arangocpp::ArangoDBConnection data = arangocpp::connectFromConfig( "examples-cfg.json" );
        // Create database connection
        arangocpp::ArangoDBGraphAPI grconnect{data};

        // If graph not exist it would be created
        grconnect.createGraph( graph, edgeDefinition );
        // Test exist graph
        std::cout << "Graph : " << graph << " exist " << grconnect.existGraph(graph) <<  std::endl;

        // Get all graphs list
        //auto graphnames = grconnect.graphNames(  );
        //printData( "Get all graphs list : ", graphnames );

        // Adds a vertex collection to the set of collections of the graph.
        //grconnect.addVertexGraph(graph, "newVertex" );

        // Adds an additional edge definition to the graph.
        //std::string edgeDefinition = " {  \"collection\" : \"newEdge\","
        //                             "     \"from\" : [ \"newVertex\" ],"
        //                             "     \"to\" :   [ \"" + vertex2 + "\"  ] }  ";
        //grconnect.addEdgeGraph(graph, edgeDefinition);

        // Fetches all (edges/vertexes) collections from the graph.
        auto vertexnames =  grconnect.graphCollectionNames( graph, "vertex" );
        printData( "Get all vertex list : ", vertexnames );
        auto edgenames =  grconnect.graphCollectionNames( graph, "edge" );
        printData( "Get all edge list : ", edgenames );

        // CRUD ----------------------------------------------------------------------

        std::string id1, id2,id3;
        if( true )
        {
        std::string record1 = "{ \"_key\": \"v_from\", \"name\": \"vertex\" , \"a\": 1  }";
        id1 = grconnect.createRecord( graph, "vertex", vertex1, record1 );
        std::string record2 = "{ \"_key\": \"v_to\", \"name\": \"vertex\" , \"b\": 2  }";
        id2 = grconnect.createRecord( graph, "vertex", vertex2, record2 );


        std::string edgerecord = "{ \"_key\": \"g_11\", \"_from\": \""+ id1 +"\",  \"_to\": \""+id2+"\", \"ab\": 12  }";
        id3 = grconnect.createRecord( graph, "edge", edge12, edgerecord );
        }
        else
        {
            id1= vertex1+"/v_from";
            id2= vertex2+"/v_to";
            id3= edge12+"/g_11";

            std::string record1 = "{ \"_key\": \"v_from\", \"name\": \"vertex\" , \"a\": 21  }";
            std::string edgerecord = "{ \"_key\": \"g_11\", \"_from\": \""+ id1 +"\",  \"_to\": \""+id2+"\", \"ab\": 212  }";
            grconnect.updateRecord( graph, "vertex", vertex1, id1, record1 );
            grconnect.updateRecord( graph, "edge", edge12, id3, edgerecord );
        }

        std::string record;
        grconnect.readRecord( graph, "vertex", vertex1, id1, record );
        std::cout << "Read vertex : \n" << record <<  std::endl;
        grconnect.readRecord( graph, "edge", edge12, id3, record );
        std::cout << "Read edge : \n" << record <<  std::endl;

        // grconnect.deleteRecord( graph, "edge", edge12, id3 ); deleted with vertex

        grconnect.deleteRecord( graph, "vertex", vertex1, id1 );
        grconnect.deleteRecord( graph, "vertex", vertex2, id2 );

        // Delete an existing graph.
        //grconnect.removeGraph( graph );
        std::cout <<  "Finish test " <<  std::endl;

    }
    catch(arangocpp::arango_exception& e)
    {
        std::cout << "TDBJsonDocument API" << e.header() << e.what() <<  std::endl;
    }
    catch(std::exception& e)
    {
        std::cout <<  "TDBJsonDocument API" << " std::exception" << e.what() <<  std::endl;
    }
    catch(...)
    {
        std::cout <<  "TDBJsonDocument API" << " unknown exception" <<  std::endl;
    }

    return 0;
}
