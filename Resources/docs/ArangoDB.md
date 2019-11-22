

## Installing ArangoDB on [macOS](https://www.arangodb.com/docs/stable/installation-mac-osx.html)


```sh
> brew install arangodb
```
You can start the server by running the command:


```sh
/usr/local/Cellar/arangodb/<VERSION>/sbin/arangod &
```
If you upgrade to new version arangodb run the command:


```sh
/usr/local/Cellar/arangodb/<VERSION>/sbin> ./arangod --database.auto-upgrade true
```



// curl -X GET @- --dump - http://localhost:8529/_api/document/test/eCRUD2 -u root:""
// curl -X HEAD --dump - http://localhost:8529/_api/document/test/eCRUD2 -u root:""
// curl -I --dump - http://localhost:8529/_api/document/test/eCRUD2 -u root:""
//  https://serverfault.com/questions/140149/difference-between-curl-i-and-curl-x-head


/*

curl -X PUT --data-binary @- --dump - https://db.cemgems.app/_db/cemgems/_api/simple/all -u backend:"cement consumption in megatons" <<EOF
{ "collection": "anonym" }
EOF

curl -u username:password http://example.com

curl -u adminrem:"password" https://db.thermohub.net/_db/hub_test/_api/collection/docpages/properties

curl -u adminrem:"password" https://db.thermohub.net/_db/hub_test/_api/collection/elements/properties

curl -u adminrem:"password" https://db.thermohub.net/_db/hub_test/_api/collection

*/
