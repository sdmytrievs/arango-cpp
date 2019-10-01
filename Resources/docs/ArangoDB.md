

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


