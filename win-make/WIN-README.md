# jsonio17


## Build and Run jsonio17 on Windows 10

* Make sure you have git installed. If not, install it on Windows: https://git-scm.com/download/win.
* To download jsonio17 source code, using Windows Command Prompt go to C:/gitJsonio and execute

```
git clone git@bitbucket.org:gems4/jsonio17.git
```

## Prepare building tools mingw


## Prepare building tools MSVC

* jsonio17 dependencies will be compiled using MSVC 2017 64 bit compiler. For this Visual Studio Community (2017) needs to be installed: 
https://docs.microsoft.com/en-us/visualstudio/install/install-visual-studio?view=vs-2017
At Step 4 - Select workloads, select Desktop development with C++ to be installed. On the individual components page check that also Windows 10 SDK is selected to be installed.
* In addition to MSVC 2017, Qt needs to be installed: https://www.qt.io/download in C:/Qt folder (Qt installation folder is used in further scripts, please use C:/Qt)!
Select with Qt 5.12.0 MSVC 2017 64-bit with Qt Charts, and Qt WebEngine.

### Install Dependencies

* For compiling the libraries that ThermoFun GUI is dependent on, three .bat scripts can be found in /thermofun. The process will several minutes. In a windows Command Prompt terminal go to C:/git/THERMOFUN/thermofun and run:

```
C:\git\THERMOFUN\thermofun>a-build-win-dependencies.bat
```

* This script builds curl and velocypack libraries, copies then in the C:\git\THERMOFUN\dependencies folder, creates buil-fun-gui folder and copies there the necessary resources files

```
C:\git\THERMOFUN\thermofun>b-build-win-boost.bat 
```

* This script builds the necessary boost libraries and copies then in the C:\git\THERMOFUN\dependencies folder

```
C:\git\THERMOFUN\thermofun>c-build-win-jsonio-jsonui.bat C:\Qt\5.12.3\msvc2017_64\bin
```

* This script builds jsonio and jsonui libraries, copies then in the C:\git\THERMOFUN\dependencies folder. Don't forget to use the corect Qt installation path.

### Compiling and the ThermoFun GUI demo in Qt Creator

* In Qt Creator open C:\git\THERMOFUN\thermofun\fungui\ThermoFunDemoGUI.pro
* Set the build folder to C:\git\THERMOFUN\build-fun-gui (release mode). NOT! C:\git\THERMOFUN\build-fun-gui\release 
* After the successful compilation try to run ThermoFun GUI from Qt Creator. All necessary dependencies and Resources should be already set in the right place. 

* Unsuccessful attempts could be due to unsuccessful compilation of dependencies, missing or not correctly copied lib or Resources files, etc. 
