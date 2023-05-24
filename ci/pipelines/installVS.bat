call activate jsonarango

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"


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
rd /s /q tmp_velo

echo "Configuring..."
cmake -G"Visual Studio 16 2019" -A x64 -S . -B build
echo "Building..."
cmake --build build --target install
