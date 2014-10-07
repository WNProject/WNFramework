#!/bin/bash
declare -a arr=(Debug Release)
MACHINE_TYPE=`uname -m`
mkdir -p ../../Linux/
INSTALL_DIR=`readlink -f ../../Linux/`/
cd ../
autoreconf
cd WNBuild
for i in ${arr[@]}
do
    if [ ${MACHINE_TYPE} == 'x86_64' ]; then
        mkdir -p ${INSTALL_DIR}x86_64/$i
        mkdir -p x86_64/$i
        cd x86_64/$i
        ../../../configure --prefix=${INSTALL_DIR}x86_64/$i --enable-shared=no
        make && make install
        cd ../../
        mkdir -p ${INSTALL_DIR}x86/$i
        mkdir -p x86/$i
        cd x86/$i
        LDFLAGS=-m32 CFLAGS=-m32 CXXFLAGS=-m32 ../../../configure --prefix=${INSTALL_DIR}x86/$i --enable-shared=no
        make && make install
        cd ../../
    else
        mkdir -p ${INSTALL_DIR}x86/$i
        mkdir -p x86/$i
        cd x86/$i
        ../../../configure --prefix=${INSTALL_DIR}x86/$i --enable-shared=no
        make && make install
        cd ../../
    fi
done
