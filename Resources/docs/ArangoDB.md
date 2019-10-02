

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
