# arango-cpp


## Build and Run arango-cpp on Windows 10

* Make sure you have git installed. If not, install it on Windows: https://git-scm.com/download/win.
* To download jsonArango source code, using Windows Command Prompt go to C:/gitJsonio and execute

```sh
git clone --recurse-submodules https://github.com/sdmytrievs/arango-cpp.git
```

## Prepare building tools mingw


## Prepare building tools MSVC

* arango-cpp dependencies will be compiled using MSVC 2017 64 bit compiler. For this Visual Studio Community (2017) needs to be installed: 
https://docs.microsoft.com/en-us/visualstudio/install/install-visual-studio?view=vs-2017
At Step 4 - Select workloads, select Desktop development with C++ to be installed. On the individual components page check that also Windows 10 SDK is selected to be installed.


### Install Dependencies

* For compiling the libraries that jsonArango is dependent on, .bat script can be found in /win-make. The process will several minutes. In a windows Command Prompt terminal go to ~jsonarango/win-make and run:

```
arango-cpp/win-make>install-dependencies-Win-msvc.bat
```

This script builds curl library, copies it in the C:\usr\local folder.


