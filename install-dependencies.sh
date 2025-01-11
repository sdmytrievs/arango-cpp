#!/bin/bash
# Installing dependencies needed to build jsonArangoDB

if [ "$(uname)" == "Darwin" ]; then

    # Do under Mac OS X platform
    #Needs Xcode and ArangoDB server locally installed
    brew upgrade
    brew install cmake
    #brew install arangodb
    CXXSTANDARD=20
    
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then

    #Needs gcc v.5 or higher and ArangoDB server locally installed
    sudo apt-get update
    sudo apt-get install -y libcurl4-openssl-dev
    CXXSTANDARD=20

fi

# Uncomment what is necessary to reinstall by force 
#sudo rm -rf /usr/local/include/spdlog
#sudo rm -f /usr/local/lib/libvelocypack.a

threads=3

# spdlog
# if no spdlog installed in /usr/local/include/spdlog (copy only headers)
test -d /usr/local/include/spdlog || {

        # Building spdlog library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone https://github.com/gabime/spdlog -b v1.11.0  && \
                cd spdlog/include && \
                sudo cp -r spdlog /usr/local/include

        # Removing generated build files
        cd ~ && \
                 rm -rf ~/code
}

# Velocypack from ArangoDB (added for installing JSONIO database client)
# if no VPack installed in /usr/local/lib/libvelocypack.a (/usr/local/include/velocypack)
test -f /usr/local/lib/libvelocypack.a || {

	# Building velocypack library
	mkdir -p ~/code && \
		cd ~/code && \
                git clone https://github.com/arangodb/velocypack.git -b main
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
