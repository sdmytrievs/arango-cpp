#!/bin/bash
# Installing dependencies needed to build ThermoFun on (k)ubuntu linux 16.04 or 18.04

if [ "$(uname)" == "Darwin" ]; then
    # Do under Mac OS X platform
    brew install cmake
fi

# Uncomment what is necessary to reinstall by force 
sudo rm -f /usr/local/lib/libvelocypack.a

threads=3

# Velocypack from ArangoDB (added for installing JSONIO database client)
# if no VPack installed in /usr/local/lib/libvelocypack.a (/usr/local/include/velocypack)
test -f /usr/local/lib/libvelocypack.a || {

	# Building velocypack library
	mkdir -p ~/code && \
		cd ~/code && \
		git clone https://github.com/arangodb/velocypack.git && \
		cd velocypack && \
		mkdir -p build && \
		cd build && \
		cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DBuildVelocyPackExamples=OFF && \
		make -j $threads && \
		sudo make install

	# Removing generated build files
	cd ~ && \
		 rm -rf ~/code
}

if [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
   sudo ldconfig
fi
