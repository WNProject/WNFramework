#!/bin/bash
$WN_NDK_TOOL_PATH/toolchains/arm-linux-androideabi-4.7/prebuilt/linux-x86_64/bin/arm-linux-androideabi-addr2line -C -f -p -e obj/local/armeabi/lib##LIB_NAME##.so < $1 
