#!/bin/sh
cd llvm
git stash && git stash drop
git apply ../llvm-android.patch
cd ..
test -d bin || mkdir -p bin
cd bin
test -d Android || mkdir -p Android 
cd Android 
test -d arm || mkdir arm
cd arm
test -d Release || mkdir Release
cd Release
pwd
CXX=${ANDROID_STATIC_TOOLCHAIN_LOC}/bin/arm-linux-androideabi-g++ \
  CC=${ANDROID_STATIC_TOOLCHAIN_LOC}/bin/arm-linux-androideabi-gcc \
  ../../../../llvm/configure --enable-optimized \
  --target=arm-none-linux-androideabi --host=x86-unknown-linux \
  --build=arm-none-linux-androideabi --enable-jit \
  --with-default-sysroot=${ANDROID_STATIC_TOOLCHAIN_LOC} \
  --enable-targets=arm

make -j16
cd ../../../
cd llvm
git stash && git stash drop
cd ..
