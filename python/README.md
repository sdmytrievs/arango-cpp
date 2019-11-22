# Building and running ArangoDB server #

API database server access. Concerning the ThermoHub database, a unification of access is needed - to database as a server locally or remotely. Clients (ThermoFun, ThermoMatch) should communicate with the server via Thrift.


## Before running ArangoDB server  ##

* Install ArangoDB Community Edition on Ubuntu

* Install python3.

~~~
sudo apt-get install python3
~~~

* Install PyArango python driver.

~~~
 sudo apt install python3-pip
 pip3 install pyarango --user
~~~

* If you have error message "ImportError: No module named Thrift" This can be easily fixed by going to the directory lib/py on the thrift  installation directory and run the following command to install the thrift module to.

~~~
 sudo python3 setup.py install
~~~

### Running ArangoDB server  ###


* Raning ArangoDB server requires python3. Navigate to the directory where this README file is located and type in terminal:

~~~
cd THERMOHUB/arangodb/py-server/

~~~

* Before start the ArangoDB server configuration file must be defined (see example ~Resources/config/mongo-server.json)

~~~
{
    "serverEndpoint": "http://localhost:8529",
    "serverUsername": "__put_here_the_user_name__",
    "serverPassword": "__put_here_the_user_password__",
    "serverDatabase": "hub_test",
     "host_name" :   "localhost",
     "port" :   9090,
     "protocol" :   "-b",
     "transport" :   "-s"
}
~~~

* Start the ArangoDB server for CRUD operation. (Ctr-C - stop server)

~~~
python3 arangodbserver.py

~~~


* To connect to ArangoDB server from bsonio-test. Select Server Socked to connect and type port "30303" 

