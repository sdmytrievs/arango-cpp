
## How to install the jsonArango library on Windows

* Make sure you have Qt and git installed. 

* Add symlink make to mingw32-make  ( do it as Admin, "Command prompt" right mouse bottom run as admin ) 

```sh
cd  C:/Qt5/Tools/mingw730_64/bin
mklink make.exe mingw32-make.exe
```

* Download jsonArango source code using git clone command:

```sh
git clone https://bitbucket.org/gems4/jsonarango.git
```

* Install cmake, curl. In "Command prompt" execute the following"


```sh
cd jsonarango\build-win
powershell -ExecutionPolicy ByPass ".\Install-win64-mingw.ps1 -installPrefix "C:\usr" "
```

* Install/Update Dependencies

In order to build the jsonArango library on Windows, execute the following:

```sh
powershell -ExecutionPolicy ByPass ".\Update-win64-mingw.ps1 -installPrefix "C:\usr" -mingwPath "C:\Qt5\Tools\mingw730_64\bin" "
```


> Before execute program, you need copy from  "C:\usr" files libcrypto-1_1-x64.dll, libssl-1_1-x64.dll and libcurl-x64.dll to executable ( see openssl-1.1.1d_2-win64-mingw.zip and "C:\usr\local\bin")
