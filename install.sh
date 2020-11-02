#!/bin/bash

#./install-dependencies.sh

BRANCH_JSON=master
BuildType=Release
InstallPrefix=/usr/local
workfolder=${PWD}


mkdir -p build
cd build
cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=$BuildType -DCMAKE_INSTALL_PREFIX=$InstallPrefix 
make 
sudo make install

if [ `uname -s` == Linux* ];  then
   sudo ldconfig
fi
