#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import json
sys.path.append('../../thrift/gen-py')

from pyArango.connection import *
from dbserver.ttypes import *
from dbserver.constants import *
from query.ttypes import *
from query.constants import *

class DBServerHandler:
    """
    Note about collection name:
    Thrift string type translates to std::string in C++ and String in
    Java. Thrift does not validate whether the string is in utf8 in C++,
    but it does in Java. If you are using this class in C++, you need to
    make sure the map name is in utf8. Otherwise requests will fail.
    """
    def __init__(self, database ="_system",  arangoURL="http://localhost:8529", username="root", password=""):
        #self.conn = Connection(username="root", password="")
        print( database, username, password )
        self.conn = Connection(arangoURL, username, password )
        self.db = self.conn[database]

    def getCollection(self, name, ctype):
        try:
            collection = self.db.collections[name]
        except KeyError:
            if ctype == "edge":
                 collection = self.db.createCollection(name=name, className='Edges')
            else:
                 collection = self.db.createCollection(name=name )
        return collection

    def getCollectionFromId( self, colName, id ):
        path = id.split("/")
        if len(path) > 1:
          return path[0], path[-1]
        else:
          return colName, path[-1]

    def getIdfromKey(self, colName, key):
        return   colName+"/"+key


    def addCollectionToId( self, colName, id ):
          path = id.split("/")
          if len(path) > 1:
            return id, path[-1]
          else:
            return colName+"/"+id, path[-1]

    def getObject( self, colName, jsonobj ):
        """ Read data from json and convert fields """
        key = None
        vobj = json.loads(jsonobj)
        if "_id" in vobj:
            if "_key" not in  vobj:
                colName, vobj["_key"] = self.getCollectionFromId( colName, vobj["_id"] )
            del vobj["_id"]
        #print( "getObject", json.dumps(vobj))
        return colName, vobj

    def setObject( self, colName, vobj ):
        """ Save data to json and convert fields """
        jsonobj =  json.dumps(vobj)
        #print( "setObject", jsonobj)
        return jsonobj

    def getField( self, doc, fieldName, vobj ) :
        vres = vobj
        v = doc
        path = fieldName.split(".")
        for k in path:
           try:
               if type(v) is dict:
                   v = v[k]
               else:
                   v = v[int(k)]
           except KeyError :
               v = None
        if v is not None:
           for i in range(len(path)-1):
               if path[i] not in vres:
                   vres[path[i]] = {}
               vres = vres[path[i]]
           vres[path[-1]] = v

    def ExtractFields( self, doc, fields ):
        """ Extract fields from document     """
        doc = self.getStore(doc)
        vobj = {}
        for field in fields:
           self.getField( doc, field, vobj)
        jsonvalue = self.setObject( None, vobj )
        return jsonvalue;

    def SelectAll(self, colName ):
        """  Returns list all records in a collection. """
        records = []
        collection = self.db.collections[colName]
        for doc in collection.fetchAll():
            object = self.getStore(doc)
            jsonval = self.setObject(colName, object)
            # print( "fetchAll", jsonval)
            records.append(DBRecord(doc._id, jsonval))
        return records

    def SelectByExample(self, colName, jsonexample):
        """  Returns list of records by query in a collection. """
        records = []
        collection = self.db.collections[colName]
        col, exmpldic = self.getObject(colName, jsonexample)
        for doc in collection.fetchByExample(exmpldic, 500):
            object = self.getStore(doc)
            jsonval = self.setObject(colName, object)
            # print( "fetchByExample", jsonval)
            records.append(DBRecord(doc._id, jsonval))
        return records

    def SelectAQL(self, colName, recordQuery):
        """  Returns list of records by query in a collection. """
        records = []
        return records

    def SelectQuery(self, colName, recordQuery):
        """  Returns list of records by query in a collection. """

        #print("SearchRecordsCollection ",colName, recordQuery )
        records = []
        if recordQuery.style == QueryStyle.QUndef or recordQuery.style == QueryStyle.QAll:
            records = self.SelectAll(colName)
        elif recordQuery.style == QueryStyle.QTemplate:
            records = self.SelectByExample(colName, recordQuery.find)
        elif recordQuery.style == QueryStyle.QEJDB:
            pass
        else:  # QAQL, QEdgesAll, QEdgesFrom, QEdgesTo
            records = self.SelectAQL(colName, recordQuery)
        return records


    def getStore(self, doc):
        """ Retrive one record from the collection"""
        object = doc.getStore()
        object["_key"] = doc._key
        object["_id"] =  doc._id
        object["_rev"] = doc._rev
        if doc.typeName == "ArangoEdge":
                object["_to"] =  doc._to
                object["_from"] = doc._from
        return object


#----------------------------------------------------------------------------------------
    def ping(self):
        """
        Pings DBServer.

        @return Success - if ping was successful.
                Error -   if ping failed.
        """
        print("Pings ArangoDBServer")
        return ResponseCode.Success;

    def stop(self):
        """
        Stop DBServer.

        """
        print("Stop ArangoDBServer")
        pass

    def addCollection(self, colName, ctype):
        """
        Add a new collection of records.

        A record is a string key / json string value pair.
        A key uniquely identifies a record in a collection.

        @param  colName collection name
        @param  ctype   collection type  (Vertex or Edge)
        @return Success - on success.
                NameExists - collection already exists.
                Error - on any other errors.

        Parameters:
         - colName
         - ctype
        """
        print("addCollection ArangoDBServer")
        try:
            collection = self.db.collections[colName]
            return ResponseCode.Success;
        except KeyError:
            collection = self.getCollection( colName, ctype )
            return ResponseCode.Success;
        except:
            return ResponseCode.Error;

    def dropCollection(self, colName):
        """
        Drops a collection of records.

        @param colName collection name
        @return Ok - on success.
                NameNotFound - map doesn't exist.
                Error - on any other errors.

        Parameters:
         - colName
        """
        print("dropCollection ArangoDBServer")
        try:
            collection = self.db.collections[colName]
            collection.delete()
            return ResponseCode.Success;
        except KeyError:
            return ResponseCode.NameNotFound;
        except:
            return ResponseCode.Error;

    def listCollections(self, ctype):
        """
        List the collections.

        @param  ctype   collection type  (Vertex or Edge or All )
        @returns StringListResponse
                     responseCode Success  or
                                  Error - on error.
                     values - list of collection names.

        Parameters:
         - ctype
        """
        print("List all the collections ArangoDBServer")
        responseCode=ResponseCode.Success
        names=[]
        errormsg = None
        try:
            colsdict = self.db.collections
            for key in colsdict:
                if colsdict[key].isSystem == True:
                    pass
                elif ctype == CollectionType.CAll :
                    names.append(key)
                elif (ctype == CollectionType.CDocument or ctype == CollectionType.CVertex) and colsdict[key].getType() == "document":
                    names.append(key)
                elif ctype == CollectionType.CEdge and colsdict[key].getType() == "edge":
                    names.append(key)
        except Exception as err:
            responseCode=ResponseCode.Error;
            errormsg = str(err)
        return StringListResponse(responseCode, names, errormsg)


    def CreateRecord(self, colName, value):
        """
        Add new record into the collection

        @param colName       collection name
        @param value         record value to save
        @returns
                 responseCode - Success
                                NameNotFound collection doesn't exist.
                                RecordExists
                                Error
                     value -   oid of the record.

        Parameters:
         - colName
         - value
        """
        print("CreateRecord ArangoDBServer")
        responseCode = ResponseCode.Success
        key = None
        errormsg = None
        try:
            newColName, vobj = self.getObject( colName, value )
            if newColName is not None:
               colName = newColName
            if "_key" in vobj:
                key = vobj["_key"]

            collection = self.db.collections[colName]
            try:
                doc = collection[key]
                responseCode = ResponseCode.RecordExists;
                key = self.getIdfromKey( colName, key)
                print( key + " Record Exists" )
            except KeyError:
                doc = collection.createDocument(vobj)
                print (doc)
                if key is not None:
                   doc["_key"] = key
                doc.save()
                key = doc._id
        except KeyError as kerr:
            responseCode = ResponseCode.NameNotFound;
            errormsg = str(kerr)
            print( errormsg )
        except Exception as err:
            responseCode = ResponseCode.Error;
            errormsg = str(err)
            print( errormsg )
        finally:
            return KeyResponse(responseCode, key, errormsg)


    def ReadRecord(self, colName, key):
        """
        Retrive one record from the collection

        @param colName       collection name
        @param key           records key to retrive.
        @returns BinaryResponse
                     responseCode - Success
                                    NameNotFound collection doesn't exist.
                                    RecordNotFound record doesn't exist.
                                    Error on any other errors.
                     value - value of the record.

        Parameters:
         - colName
         - key
        """
        print("ReadRecord ArangoDBServer")
        responseCode = ResponseCode.Success
        value = None
        errormsg = None
        try:
            newColName, key = self.getCollectionFromId( colName, key )
            if newColName is not None:
               colName = newColName

            collection = self.db.collections[colName]
            try:
                doc = collection[key]
                object = self.getStore( doc )
                value = self.setObject( colName, object)
            except KeyError as derr:
                responseCode = ResponseCode.RecordNotFound;
                errormsg = str(derr)
                print( errormsg )

        except KeyError as kerr:
            responseCode = ResponseCode.NameNotFound;
            errormsg = str(kerr)
            print( errormsg )
        except Exception as err:
            responseCode = ResponseCode.Error;
            errormsg = str(err)
            print( errormsg )
        finally:
            return ValueResponse(responseCode, value, errormsg)


    def UpdateRecord(self, colName, key, value):
        """
        Update record in the collection

        @param colName       collection name
        @param key           records key to update.
        @param value         record value to update
        @returns Success
                 NameNotFound collection doesn't exist.
                 RecordNotFound
                 Error

        Parameters:
         - colName
         - key
         - value
        """
        print("UpdateRecord ArangoDBServer")
        try:
            newColName, vobj = self.getObject( colName, value )
            #if newColName is not None:
            #   colName = newColName

            newColName, key = self.getCollectionFromId( colName, key )
            if newColName is not None:
               colName = newColName

            collection = self.db.collections[colName]
            try:
                doc = collection[key]
                for fild in doc.getStore():
                  if fild not in vobj:
                    del doc[fild]
                doc.set(vobj)
            except KeyError:
                doc = collection.createDocument(vobj)
            if key is not None:
               doc["_key"] = key
            doc.save()
            return ResponseCode.Success
        except KeyError as kerr:
            print( str(kerr))
            return ResponseCode.NameNotFound;
        except Exception as err:
            print( str(err))
            return ResponseCode.Error;

    def DeleteRecord(self, colName, key):
        """
        Removes record from the collection

        @param colName       collection name
        @param key           records key to delete.
        @returns Success
                 NameNotFound collection doesn't exist.
                 RecordNotFound
                 Error

        Parameters:
         - colName
         - key
        """
        print("DeleteRecord ArangoDBServer")
        try:
            newColName, key = self.getCollectionFromId( colName, key )
            if newColName is not None:
               colName = newColName
            collection = self.db.collections[colName]
            try:
                doc = collection[key]
                doc.delete()
                return ResponseCode.Success
            except KeyError as err:
                print(str(err))
                return ResponseCode.RecordNotFound
        except KeyError as err:
            print( "!!!"+str(err))
            return ResponseCode.NameNotFound
        except Exception as err:
            print( str(err) )
            return ResponseCode.Error

    def SearchRecords(self, colName, fields, recordQuery):
        """
        Returns list of records in a collection.

        @param colName       collection name
        @param fields - list of fileds selection
        @param recordQuery   json query string for collection
        @return RecordListResponse
                    responseCode - Success
                                 - NameNotFound collection doesn't exist.
                                 - Error on any other errors
                    records - list of records.

        Parameters:
         - colName
         - fields
         - recordQuery
        """
        print("SearchRecords ArangoDBServer ",colName, recordQuery )
        responseCode = ResponseCode.Success
        records = []
        errormsg = None
        try:
            records = self.SelectQuery( colName, recordQuery)
            print(len(records))
        except KeyError as kerr:
            responseCode = ResponseCode.NameNotFound;
            errormsg = str(kerr)
        except Exception as err:
            responseCode = ResponseCode.Error;
            errormsg = str(err)
        finally:
            return DBRecordListResponse(responseCode, records, errormsg)

    def DeleteEdges(self, colName, vertexkey):
        """
        Delete edges connected to vertex record

        @param colName       vertex collection name
        @param vertexkey     vertex key
        @returns Success
                 NameNotFound collection doesn't exist.
                 Error

        Parameters:
         - colName
         - vertexkey
        """
        print("Delete list of records ArangoDBServer")
        return ResponseCode.Success

    def CollectValues(self, colName, fpath):
        """
        Provides 'distinct' operation over collection

        @param colName       collection name
        @param fpath         Field path to collect distinct values from.
        @return StringListResponse
                    responseCode - Success
                                 - NameNotFound collection doesn't exist.
                                 - Error on any other errors
                    names - list of distinct values filed fpath.

        Parameters:
         - colName
         - fpath
        """
        print("SearchValues ArangoDBServer")
        pass

    def GenerateId(self, colName, _keytemplate):
        """
         Generate new unique oid or other key string

        @param colName       collection name
        @returns new key string

        Parameters:
         - colName
         - _keytemplate
        """
        print("GenerateId ArangoDBServer")
        if _keytemplate is not None:
             key = self.getIdfromKey(colName, _keytemplate)
        return key

print("Init DBServerHandler...")

if  __name__ == '__main__':
    handler = DBServerHandler()
#    ret = handler.dropCollection("test")
#    print( ret )
#    ret = handler.addCollection("test")
#    print( ret )
    ret = handler.listCollections()
    print( ret )
#    print( "listCollections return {0}: number of collections {1}".format( ret.responseCode, len( ret.names ) ))
#    ret = handler.ReadRecord("substance","584532e1e409385d000002ef")
    ret = handler.ReadRecord("test","test11")
    print( ret )
    print( "ReadRecord return {0}: value {1}".format( ret.responseCode, ret.value ))
    ret = handler.DeleteRecord("test","test22")
    print( "DeleteRecord return {0}".format( ret ))
    ret = handler.CreateRecord("test",'{ "a": 1, "gg": 2 }')
    print( ret )
    ret = handler.UpdateRecord("test", "testbb", '{ "_id": "testbb", "tt": 1, "gtt": 4, "a": { "c": 55 } }')
    print( ret )
    ret = handler.CreateRecord("test",'{ "_id": "testcc", "c": 1, "gc": 2 }')
    print( ret )
    ret = handler.GenerateId("test")
    print( ret )

    print( "----------------------------------------------" )

    ret = handler.SearchRecords( "substance", ["_key", "_id", "properties.symbol"], "" )
    print( ret )

    print( "----------------------------------------------" )

    ret = handler.SearchRecords( "airports", ["_key", "airport", "city", "state" ], '{ "state": "MO" }' )
    print(ret)
    #ret = handler.SearchRecords( "airports", ["_key", "airport", "city", "state" ], '{ "_key": "RBE" }' )
    for rec in ret.records:
      print( rec )

    print("done!")


