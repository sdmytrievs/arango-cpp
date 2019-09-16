#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ImportError: No module named Thrift
# This can be easily fixed by going to the directory lib/py on the thrift
# installation directory and run the following command to install the thrift module to your python libraries:
# sudo python setup.py install
# sudo python3 setup.py install

import sys
import json
from arangodbhandler import *
sys.path.append('../../thrift/gen-py')

from dbserver import DBServer
from dbserver.ttypes import *
#from dbserver.constants import *

from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol
from thrift.server import TServer

cfgdata = {
            "serverEndpoint": "http://localhost:8529",
            "serverUsername": "root",
            "serverPassword": "",
            "serverDatabase": "test55",
            "host_name": "localhost",
            "port": 9090,
            "protocol": "-b",
            "transport": "-s"
          }
with open( "arango-server.json" ) as fd:
     cfgdata.update( json.load(fd) )
print(cfgdata)

handler = DBServerHandler( database =cfgdata[ "serverDatabase"],
                           arangoURL=cfgdata[ "serverEndpoint"],
                           username=cfgdata[ "serverUsername"],
                           password=cfgdata[ "serverPassword"])
#handler.dropCollection("edge")
#handler.getCollection("vertex")
#handler.getCollection("edge")
#handler.getCollection("docpages")
#handler.getCollection("impexdefs")
#handler.getCollection("queries")

processor = DBServer.Processor(handler)
transport = TSocket.TServerSocket(port=cfgdata[ "port"])
tfactory = TTransport.TBufferedTransportFactory()
pfactory = TBinaryProtocol.TBinaryProtocolFactory()

server = TServer.TSimpleServer(processor, transport, tfactory, pfactory)
ret = handler.listCollections(CollectionType.CVertex)
print( ret )

print("Starting python server...")
server.serve()
print("done!")


