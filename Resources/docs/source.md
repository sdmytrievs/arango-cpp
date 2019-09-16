## Files descriptions

1. exeptions.h

General exception structure into project.

> _source: "+";    test: "-";  example: "-"_


2. jsondetail.h(cpp)  

    - Type check macros  ( is_container, is_mappish )
    - Basic functions for read/write numeric values to/from string
    - Split string to query functions

> *source: "+";    test: "+";  example: "-"*



## Coments

To Do

1. Make all files descriptions

2. Add google tests for driver (research before best practices )

3. Add time test for driver

4. Work about time decrements  (research methods and curl problems)

    - curl could be init and only chage curl requests
    - All options could not be set all time: user pathword ...
    - no curl use
    - ...

5. Add the same test examples for pyArango and compare time execution

6. Into ArangoDBCollectionAPI no example for queryEdgesToFrom and removeEdges
   more detail implement and test graph commands
   https://www.arangodb.com/docs/stable/aql/graphs.html




