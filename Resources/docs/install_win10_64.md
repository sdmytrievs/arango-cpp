## Addition information ##

### Open Command Prompt in Windows 10 ###

   1. Select the Start button.
   2. Type "cmd".


### Commands into "Command Prompt" ###

~~~

 > dir                  
 > cd ..                 
 > cd  C:/Qt5/Tools/mingw730_64/bin

~~~~

### Open Environment Variables in Windows 10 ###

( or under Control Panel->System->Advanced )

   1. Select the Start button.
   2. Type "var".


### use default Working dir ###

C:/usr/local


### git editor ###
https://apple.stackexchange.com/questions/252541/how-do-i-escape-the-git-commit-window-from-os-x-terminal

## Main Install part ##

### Install cmake ###

1. Download from "https://cmake.org/download/"
2. Unpack to  "C:/usr"
3. Add C:\usr\cmake-3.14.0-rc1-win64-x64\bin to you PATH under Control Panel->System->Advanced.
   
~~~
cmake --version
cmake --system-information a.aa

~~~

### Link cmake to qt mingw dir ###

1. Add C:/Qt5/Tools/mingw730_64/bin to you PATH under Control Panel->System->Advanced.
2. For convenience, symlink make to mingw32-make
   ( do it as Admin, "Command prompt" right mouse bottom run as admin )
~~~
cd  C:/Qt5/Tools/mingw730_64/bin
mklink make.exe mingw32-make.exe
~~~

## Install curl ##

https://develop.zendesk.com/hc/en-us/articles/360001068567-Installing-and-using-cURL
https://curl.haxx.se/windows/
1. Download curl from https://curl.haxx.se/windows/
2. Unpack and copy lib, bin and include to c:/usr/local
3. Download libssh2 1.9.0 from https://curl.haxx.se/windows/
4. Download OpenSSL 1.1.1c from https://curl.haxx.se/windows/
5. copy libcrypto-1_1-x64.dll, libssl-1_1-x64.dll and libcurl-x64.dll to executable



### Build ArangoDB VelocyPack serializer as follows: ###

~~~

cd c:\Install
git clone https://github.com/arangodb/velocypack.git 
cd velocypack 
mkdir build 
cd build 
cmake -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" -DBuildVelocyPackExamples=OFF -DCMAKE_INSTALL_PREFIX=C:\usr\local .. && make && make install

~~~

