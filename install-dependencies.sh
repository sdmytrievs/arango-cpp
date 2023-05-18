#!/bin/bash
# Installing dependencies needed to build jsonArangoDB

if [ "$(uname)" == "Darwin" ]; then

    # Do under Mac OS X platform
    #Needs Xcode and ArangoDB server locally installed
    brew upgrade
    brew install cmake
    brew install spdlog
    #brew install arangodb
    CXXSTANDARD=17
    
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then

    #Needs gcc v.5 or higher and ArangoDB server locally installed
    sudo apt-get update
    sudo apt-get install -y libcurl4-openssl-dev
    sudo apt install libspdlog-dev
    CXXSTANDARD=17

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
                git clone https://github.com/gdmiron/velocypack.git -b v0.1.3 && \ # version has to be the same as used in conda, use gdmiron fork
                #git clone https://github.com/arangodb/velocypack.git -b main
                cd velocypack && \
		mkdir -p build && \
		cd build && \
                cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DBuildVelocyPackExamples=OFF -DCMAKE_CXX_STANDARD=$CXXSTANDARD && \
		make -j $threads && \
		sudo make install

	# Removing generated build files
	cd ~ && \
		 rm -rf ~/code
}

if [ `uname -s` == Linux* ]; then
   sudo ldconfig
fi
