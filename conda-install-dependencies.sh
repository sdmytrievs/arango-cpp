#!/bin/bash

# Uncomment what is necessary to reinstall by force 
# rm -f ${CONDA_PREFIX}/lib/libvelocypack.a

BRANCH_ARANGO=main

# if no JSONIO installed in ${CONDA_PREFIX}/lib/libvelocypack.a (${CONDA_PREFIX}/include/velocypack)
test -f ${CONDA_PREFIX}/lib/libvelocypack.a || {

        # Building velocypack library
	mkdir -p ~/code && \
		cd ~/code && \
                git clone https://github.com/arangodb/velocypack.git -b $BRANCH_ARANGO && \
                cd velocypack && \
		mkdir -p build && \
		cd build && \
                cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DBuildVelocyPackExamples=OFF && \
                #cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DBuildVelocyPackExamples=OFF -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} && \
                make  && \
		make install

	# Removing generated build files
	cd ~ && \
		 rm -rf ~/code
}

