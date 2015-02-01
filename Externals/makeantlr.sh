#!/bin/bash
cd antlr
MACHINE_TYPE=`uname -m`
mkdir -p ../bin/Linux/
mkdir -p ../bin/Android/
INSTALL_DIR=`readlink -f ../bin/Linux/`/
ANDROID_DIR=`readlink -f ../bin/Android/`/
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
    mkdir -p ${ANDROID_DIR}arm/$i 
    mkdir -p ${ANDROID_DIR}arm/$i/include
    cp -R -f *.hpp ${ANDROID_DIR}arm/$i/include/
    cp -R -f *.inl ${ANDROID_DIR}arm/$i/include/
done

cd ..
