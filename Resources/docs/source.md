## Files descriptions

1. arangoexeptions.h

General exception structure into project.

> _source: "+";    test: "+";  example: "+"_


2. arangodetail.h

Some internal functions

> *source: "+";    test: "x";  example: "internal"*

3. arangocurl.h(cpp)

Working with curl : [libcurl](https://curl.haxx.se/libcurl/c/libcurl.html) - client-side URL transfers

> *source: "+";    test: "x";  example: "internal"*

4. arangorequests.h(cpp)

Internal description of requests

> *source: "+";    test: "x";  example: "internal"*

5. arangoquery.h(cpp)

Description AQL, by Template and other ArangoDB query types and other external structures/functions

> *source: "+";    test: "+";  example: "+"*


6. arangoconnect.h(cpp)

Classes with ArangoDB  connection data (URL, user name, password, database name)

> *source: "+";    test: "+";  example: "+"*

7. arangousers.h(cpp)

API to Create/Delete ArangoDB users and databases. You need permission to the _system database in order to execute this REST call.

> *source: "+";    test: "+";  example: "+"*

8. arangocollection.h(cpp)

ArangoDBCollectionAPI the API for manipulating collections and documents within the database. The Collection API is valid for all Collection instances, regardless of their type (vertices or edges).

> *source: "+";    test: "+";  example: "+"*

9. arangograph.h(cpp)

ArangoDBGraphAPI the API for manipulating property graphs.

> *source: "+";    test: "+";  example: "+"*

## Coments

Version 0.2 Done:

1. Added time test for driver

2. Work about time decrements  (research methods and curl problems)
    - curl could be init and only chage curl requests

3. Added google tests for driver:
   - test crud   +
   - test selection +
   - test query params +
   - test local root commands +
   - test graph commands +
   - run test for remote +


To Do

4. Add the same test examples for pyArango and compare time execution

5. Into ArangoDBCollectionAPI no example for queryEdgesToFrom and removeEdges


>> more detail implement and test graph commands   https://www.arangodb.com/docs/stable/aql/graphs.html




// https://stackoverflow.com/questions/87610/automated-integration-testing-a-c-app-with-a-database
