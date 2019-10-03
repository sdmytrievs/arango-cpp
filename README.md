# jsonArango

A lightweight ArangoDB client C++ library

## What jsonArango does?

* _ArangoDBCollectionAPI_   the API for manipulating collections and documents within the database. The Collection API is valid for all Collection instances, regardless of their type (vertices or edges).
* _ArangoDBGraphAPI_   the API for manipulating property graphs.
* _ArangoDBUsersAPI_   the API for creating/deleting ArangoDB users and databases.
* 
jsonArango is written in C++11 using the open-source library Velocypack from ArangoDB.
Version: currently 0.1.
Will be distributed as is (no liability) under the terms of Lesser GPL v.3 license.

## How to install the jsonArango library

* Make sure you have g++, cmake and git installed. If not, install them (on Ubuntu Linux):

```sh
sudo apt-get install g++ cmake git libssl-dev libtool byacc flex
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
sudo ./install-dependencies.sh
```

* Install the jsonArango library

```sh
sudo ./install.sh
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

try{
    std::string collectionName = "test";

    // Set ArangoDB connection data (URL, user name, password, database name)
    arangocpp::ArangoDBConnect sysdb_data( "http://localhost:8529", "root", "", "_system");
    // Create a database connection
    arangocpp::ArangoDBCollectionAPI sysdb_connect{sys_data};

// Create a new database "mydatabase" (or get to existing one) and set a connection "mydb_connect" to it

    // If document collection collectionName does not exist, it will be created
    mydb_connect.createCollection(collectionName, "vertex");

    // Set data to document
    std::string documentData = "{ \"_key\" : \"eCRUD\", "
                               "  \"task\" : \"exampleCRUD\", "
                               "  \"properties\" : { \"level\": \"insert record\" } "
                               "}";

    // Insert a new document to database collection and retrieve its _key
    auto rkey = mydb_connect.createRecord( collectionName, documentData );

    // Read a document from database collection
    std::string readDocumentData;
    mydb_connect.readRecord( collectionName, rkey, readDocumentData);

    // modify the document in readDocumentData
    ...

    // Save the modified document to database collection
    mydb_connect.updateRecord( collectionName, rkey, readDocumentData );

    // Delete the document from database collection
    connect.deleteRecord( collectionName, rkey );
    
// Close the connection to "mydatabase"

// Close the connection to "_system" database


}
catch( arangocpp::arango_exception& e)
{
    std::cout <<  e.header() << e.what() <<  std::endl;
}
catch(std::exception& e)
{
    std::cout <<  "std::exception " << e.what() <<  std::endl;
}

```

For more information on APIs and more examples, take a look into the /examples directory.

