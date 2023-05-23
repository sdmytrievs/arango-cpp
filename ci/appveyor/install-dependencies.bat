
if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2017" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
)
if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2019" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
)

mkdir tmp_velo
cd tmp_velo

echo
echo ******                    ******
echo ****** Compiling velocypack ******
echo ******                    ******
echo

echo git clone velocypack...
git clone https://github.com/arangodb/velocypack.git -b main
cd velocypack

echo "Configuring..."
cmake -G"Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH="%CONDA_PREFIX%\Library" .. -A x64 -S . -B build
echo "Building..."
cmake --build build --config %CONFIGURATION% --target install

cd ..\..

REM Housekeeping
rd /s /q tmp_velo
