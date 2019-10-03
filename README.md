# jsonArango

A lightweight ArangoDB client C++ library

## What jsonArango does?

* _ArangoDBCollectionAPI_   the API for manipulating collections and documents within the database. The Collection API is valid for all Collection instances, regardless of their type (vertices or edges).
* _ArangoDBGraphAPI_   the API for manipulating property graphs.
* _ArangoDBUsersAPI_   the API for creating/deleting ArangoDB users and databases.
* 
jsonArango is written in _C++11_ using the open-source library Velocypack from ArangoDB.
Version: currently 0.1.
Will be distributed as is (no liability) under the terms of Lesser GPL v.3 license.

## How to install the jsonArango library

* Make sure you have g++, cmake and git installed. If not, install them (on Ubuntu Linux):

```sh
sudo apt-get install g++ cmake git libssl-dev
```
For Mac OSX, make sure you have Xcode and Homebrew installed (see [Homebrew web site](http://brew.sh) ).

* Download jsonArango source code using git clone command:

```sh
git clone https://bitbucket.org/gems4/jsonarango.git
```

* Install Dependencies

In order to build the jsonArango library on Ubuntu Linux or MacOS, first execute the following:

```sh
cd jsonarango
./install-dependencies.sh
```

* Install the jsonArango library

```sh
./install.sh
```

After that, headers and the library can be found in /usr/local/include/jsonarango, /usr/local/lib/.


### Install current version of ArangoDB server locally

* On (K)Ubuntu linux, install [from here](https://www.arangodb.com/download-major/ubuntu/):

First, add the repository key to the apt database:

```sh
curl -OL https://download.arangodb.com/arangodb35/DEBIAN/Release.key
sudo apt-key add - < Release.key
```

Then use apt-get to install arangodb:

```sh
echo 'deb https://download.arangodb.com/arangodb35/DEBIAN/ /' | sudo tee /etc/apt/sources.list.d/arangodb.list
sudo apt-get install apt-transport-https
sudo apt-get update
sudo apt-get install arangodb3=3.5.0-1
```

The future updates will come together with other ubuntu packages installed.

* On MacOS Sierra or higher, [navigate here](https://www.arangodb.com/docs/stable/installation-mac-osx.html) and follow the instructions on how to install ArangoDB with homebrew. Basically, you have to open a terminal and run two commands:

~~~
brew update
brew install arangodb
~~~

### How to use jsonArango (use cases)

```c++

#include <iostream>
#include "jsonarango/arangocollection.h"
#include "jsonarango/arangoexception.h"

int main(int, char* [])
{
    std::string databaseURL = "http://localhost:8529";
    std::string databaseName = "mydatabase";
    std::string collectionName = "mydocuments";

    try{
        // Set ArangoDB root connection data (URL, user name, password, database name)
        arangocpp::ArangoDBConnection root_data( databaseURL, "root", "", "_system");
        // Create a database root connection
        arangocpp::ArangoDBRootClient root_connect{root_data};

        // Define ArangoDB users
        arangocpp::ArangoDBUser dbuser( "myuser", "mypasswd", "rw");
        // Create a new database "mydatabase" (or get to existing one)
        root_connect.createDatabase(databaseName, {dbuser} );

        // Set ArangoDB connection data (URL, user name, password, database name)
        arangocpp::ArangoDBConnection mydb_data( databaseURL, dbuser.name, dbuser.password, databaseName);
        // Create a database connection
        arangocpp::ArangoDBCollectionAPI mydb_connect{mydb_data};


        // Create a new collection "mydocuments" (or get to existing one)
        mydb_connect.createCollection( collectionName, "vertex");

        // Set data to document
        std::string documentData = "{ \"_key\" : \"CRUD\", "
                                   "  \"baz\": [1, 2, 3], "
                                   "  \"foo\" : { \"baz\": \"boo\" } "
                                   "}";


        // Insert a new document to database collection and retrieve its handle
        auto documentHandle = mydb_connect.createDocument( collectionName, documentData );


        // Read a document from database collection
        std::string readDocumentData;
        mydb_connect.readDocument( collectionName, documentHandle,  readDocumentData);

        // Modify the document in readDocumentData
        ...

        // Save the modified document to database collection
        mydb_connect.updateDocument( collectionName, documentHandle, readDocumentData );

        // Delete the document from database collection
        mydb_connect.deleteDocument( collectionName, documentHandle );

    }
    catch(arangocpp::arango_exception& e)
    {
        std::cout << e.header() << e.what() <<  std::endl;
    }
    catch(std::exception& e)
    {
        std::cout <<  " std::exception" << e.what() <<  std::endl;
    }

    return 0;
}

```

For more information on APIs and more examples, take a look into the /examples directory.

