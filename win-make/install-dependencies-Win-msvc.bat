@echo off
setlocal EnableDelayedExpansion 

set ROOT_DIR=%cd%
set LOCALINSTALL=C:/usr/local
set PROGFILES=%ProgramFiles%
if not "%ProgramFiles(x86)%" == "" set PROGFILES=%ProgramFiles(x86)%

REM Check if Visual Studio 2017 comunity is installed
set MSVCDIR="%PROGFILES%\Microsoft Visual Studio\2017\Community"
set VCVARSALLPATH="%PROGFILES%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"        
if exist %MSVCDIR% (
  if exist %VCVARSALLPATH% (
   	set COMPILER_VER="2017"
        echo Using Visual Studio 2017 Community
	goto setup_env
  )
)

REM Check if Visual Studio 2019 comunity is installed
set MSVCDIR="%PROGFILES%\Microsoft Visual Studio\2019\Community"
set VCVARSALLPATH="%PROGFILES%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"        
if exist %MSVCDIR% (
  if exist %VCVARSALLPATH% (
   	set COMPILER_VER="2019"
        echo Using Visual Studio 2019 Community
	goto setup_env
  )
)


echo No compiler : Microsoft Visual Studio 2017 or 2019 Community is not installed.
goto end

:setup_env

echo "%MSVCDIR%\VC\Auxiliary\Build\vcvarsall.bat"
call %MSVCDIR%\VC\Auxiliary\Build\vcvarsall.bat x64

echo
echo ******                ******
echo ****** Compiling Curl ******
echo ******                ******
echo


mkdir tmp_libcurl

cd tmp_libcurl

echo Get Curl from git...

git clone https://github.com/curl/curl.git
cd curl*
git checkout tags/curl-7_62_0

cd src

rename "tool_hugehelp.c.cvs" "tool_hugehelp.c"

cd ..

cd winbuild

if %COMPILER_VER% == "2017" (
        set VCVERSION = 14.1
        goto buildnow
)

:buildnow
REM Build!

if [%1]==[-static] (
        set RTLIBCFG=static
        echo Using /MT instead of /MD
)

REM echo Compiling dll-debug-x64 version...
REM nmake /f Makefile.vc mode=dll VC=%VCVERSION% DEBUG=yes MACHINE=x64

echo Compiling dll-release-x64 version...
nmake /f Makefile.vc mode=dll VC=%VCVERSION% DEBUG=no GEN_PDB=yes MACHINE=x64

REM echo Compiling static-debug-x64 version...
REM nmake /f Makefile.vc mode=static VC=%VCVERSION% DEBUG=yes MACHINE=x64

REM echo Compiling static-release-x64 version...
REM nmake /f Makefile.vc mode=static VC=%VCVERSION% DEBUG=no MACHINE=x64

echo Copy compiled .*lib files in lib-release folder to "%LOCALINSTALL% folder
cd ..\builds\libcurl-vc-x64-release-dll-ipv6-sspi-winssl
xcopy .  "%LOCALINSTALL%" /E

echo Cleanup temporary file/folders
cd %ROOT_DIR%
rd /s /q tmp_libcurl

mkdir tmp_velo
cd tmp_velo


echo
echo ******                		 ******
echo ****** Compiling Velocypack ******
echo ******                		 ******
echo


echo Get velocypack from git...
git clone https://github.com/arangodb/velocypack.git
cd velocypack

echo "Configuring..."
REM cmake -G"Visual Studio 16 2019" .. -DCMAKE_BUILD_TYPE=Release  -DBuildVelocyPackExamples=OFF -DCMAKE_INSTALL_PREFIX=%LOCALINSTALL% .. -A x64 -S . -B build
cmake -G"Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release -DBuildVelocyPackExamples=OFF -DCMAKE_INSTALL_PREFIX=%LOCALINSTALL% .. -S . -B build
echo "Building..."
cmake --build build  --target install
cd ..\..

REM Housekeeping
rd /s /q tmp_velo



