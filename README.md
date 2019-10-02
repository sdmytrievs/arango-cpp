# jsonArango

A lightweight ArangoDB client C++ library

## What jsonArango does?

* _ArangoDBCollectionAPI_ the API for manipulating collections and documents into. The Collection API is implemented by all Collection instances, regardless of their specific type.
* _ArangoDBGraphAPI_ implement the API for manipulating graphs.
* _ArangoDBUsersAPI_ implement the API for create/delete ArangoDB users and databases.
* jsonArango is written in C++11 using open-source library Velocypack from ArangoDB.
* Version: currently 0.1.
* Will be distributed as is (no liability) under the terms of Lesser GPL v.3 license.

## How to install the jsonArango library

* Download jsonArango source code using git clone

```sh
git clone git clone git@bitbucket.org:gems4/jsonarango.git
cd jsonarango
```

* Make sure you have g++, cmake and git installed. If not, install them (on Ubuntu Linux):

```sh
sudo apt-get install g++ cmake git libssl-dev libtool byacc flex
```
For Mac OSX, make sure you have Xcode and Homebrew installed (see [Homebrew web site](http://brew.sh) ).

* Install Dependencies

In order to build the jsonArango library on Ubuntu Linux or MacOS, first execute the following:

```sh
./install-dependencies.sh
```

* Install the jsonArango library

Then navigate to the directory where this README.md file is located and type in terminal:

```sh
sudo ./install.sh
```

* After that, headers, library  and the third-party libraries can be found in /usr/local/{include,lib}.


### Install current version of ArangoDB server locally

* On (K)Ubuntu linux, install the current version of ArangoDB server locally [from here](https://www.arangodb.com/download-major/ubuntu/):

First add the repository key to apt like this:

```sh
curl -OL https://download.arangodb.com/arangodb35/DEBIAN/Release.key
sudo apt-key add - < Release.key
```

Use apt-get to install arangodb:

```sh
echo 'deb https://download.arangodb.com/arangodb35/DEBIAN/ /' | sudo tee /etc/apt/sources.list.d/arangodb.list
sudo apt-get install apt-transport-https
sudo apt-get update
sudo apt-get install arangodb3=3.5.0-1
```

The updates will come together with other ubuntu packages that you have installed.

* On MacOS Sierra or higher, [navigate here](https://www.arangodb.com/docs/stable/installation-mac-osx.html) and follow the instructions on how to install ArangoDB with homebrew. Basically, you have to open a terminal and run two commands:

~~~
brew update
brew install arangodb
~~~

### How to use jsonArango (use cases)

```c++

try{

    // Get Arangodb connection data( load settings from "examples-cfg.json" config file )
    arangocpp::ArangoDBConnect data( "http://localhost:8529", "root","", "_system");
    // Create database connection
    arangocpp::ArangoDBCollectionAPI connect{data};

    // If document collection collectionName not exist it would be created
    connect.createCollection(collectionName, "vertex");

    // Set data to document
    std::string documentData = "{ \"_key\" : \"eCRUD\", "
                               "  \"task\" : \"exampleCRUD\", "
                               "  \"properties\" : { \"level\": \"insert record\" } "
                               "}";

    // Insert new document to database
    auto rkey = connect.createRecord( collectionName, documentData );

    // Read document from database
    std::string readDocumentData;
    connect.readRecord( collectionName, rkey,  readDocumentData);

    // modify document
    ...

    // Save changed document to database
    connect.updateRecord( collectionName, rkey, documentData );

    // Delete document from database
    connect.deleteRecord( collectionName, rkey );

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

For more information, have a look at the /examples directory.
