#!/bin/bash

MACHINE_TYPE=`uname -m`
mkdir -p ../Linux/
INSTALL_DIR=`readlink -f ../Linux/`/
declare -a arr=(Debug Release)
for i in ${arr[@]}
do
    if [ ${MACHINE_TYPE} == 'x86_64' ]; then
        mkdir -p ${INSTALL_DIR}x86_64/$i
        mkdir -p ${INSTALL_DIR}x86_64/$i/include
        cp -R -f *.hpp ${INSTALL_DIR}x86_64/$i/include/
        cp -R -f *.inl ${INSTALL_DIR}x86_64/$i/include/
        mkdir -p ${INSTALL_DIR}x86/$i 
        mkdir -p ${INSTALL_DIR}x86/$i/include
        cp -R -f *.hpp ${INSTALL_DIR}x86/$i/include/
        cp -R -f *.inl ${INSTALL_DIR}x86/$i/include/
    else
        mkdir -p ${INSTALL_DIR}x86/$i 
        mkdir -p ${INSTALL_DIR}x86/$i/include
        cp -R -f *.hpp ${INSTALL_DIR}x86/$i/include/
        cp -R -f *.inl ${INSTALL_DIR}x86/$i/include/
    fi
done
