#!/bin/bash
# Installing dependencies needed to build jsonArangoDB

localPrefix=$1
full_path="$localPrefix/lib/libvelocypack.a"
threads=3

# Velocypack from ArangoDB (added for installing JSONIO database client)
# if no VPack installed in /usr/local/lib/libvelocypack.a (/usr/local/include/velocypack)
test -f $full_path || {

	# Building velocypack library
	mkdir -p ~/code && \
		cd ~/code && \
		git clone https://github.com/arangodb/velocypack.git && \
		cd velocypack && \
		mkdir -p build && \
		cd build && \
                cmake  -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" -DBuildVelocyPackExamples=OFF -DCMAKE_INSTALL_PREFIX=$localPrefix .. && \
		make -j $threads && \
		make install

	# Removing generated build files
	cd ~ && \
		 rm -rf ~/code
}
