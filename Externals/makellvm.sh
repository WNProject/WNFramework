#!/bin/sh

test -d bin || mkdir -p bin
cd bin
test -d Linux || mkdir -p Linux
cd Linux
test -d `uname -m` || mkdir `uname -m`
cd `uname -m`
test -d Release || mkdir Release
test -d Build || mkdir Build
cd Build
test -d Release || mkdir Release
cd Release
../../../../../llvm/configure --enable-optimized --disable-assertions --prefix=`pwd`/../../Release
make -j16
make install
cd ../../
test -d Debug || mkdir Debug
test -d Build || mkdir Build
cd Build
test -d Debug || mkdir Debug
cd Debug
../../../../../llvm/configure --disable-optimized --prefix=`pwd`/../../Debug
make -j16
make install
cd ../../../../../
