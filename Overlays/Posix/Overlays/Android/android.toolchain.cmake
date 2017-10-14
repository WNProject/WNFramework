# Toolchain for building Android APK in WNFramework

set(WN_ANDROID_SDK ""
    CACHE STRING "Location of the Android SDK")
if (WN_ANDROID_SDK)
    string(REPLACE "\\" "\\\\" WN_ANDROID_SDK ${WN_ANDROID_SDK})
endif()

set(WN_ANDROID_NDK "${WN_ANDROID_SDK}/ndk-bundle"
    CACHE STRING "Location of the Android NDK")
string(REPLACE "\\\\" "\\" WN_ANDROID_NDK ${WN_ANDROID_NDK})
string(REPLACE "\\" "\\\\" WN_ANDROID_NDK ${WN_ANDROID_NDK})

set(WN_ANDROID_TOOLCHAIN_FILE 
    "${WN_ANDROID_NDK}/build/cmake/android.toolchain.cmake"
    CACHE STRING "Location of the android.cmake.toolchain for the NDK")

message(STATUS "Using Android SDK ${WN_ANDROID_SDK}")
message(STATUS "Using Android NDK ${WN_ANDROID_NDK}")
message(STATUS "Using Android Toolchain ${WN_ANDROID_TOOLCHAIN_FILE}")
set(ANDROID_NDK ${WN_ANDROID_NDK})

set(WN_ANDROID_NATIVE_API_LEVEL 19
    CACHE STRING "Native API level to build")

set(WN_ANDROID_ABIS "ARM;x86;ARM64" CACHE STRING
    "Which Android ABIs to build")

set(WN_FORCED_ARCHITECTURES ${WN_ANDROID_ABIS})

set(WN_SANITIZED_ABIS)
foreach(ABI ${WN_ANDROID_ABIS})
    if (${ABI} STREQUAL ARM)
        list(APPEND WN_SANITIZED_ABIS "armeabi-v7a")
    elseif(${ABI} STREQUAL ARM64)
        list(APPEND WN_SANITIZED_ABIS "arm64-v8a")
    elseif(${ABI} STREQUAL x86)
        list(APPEND WN_SANITIZED_ABIS "x86")
    else()
        list(APPEND WN_SANITIZED_ABIS ${ABI})
    endif()
endforeach()

set(WN_ANDROID_WRAPPER ON)
set(WN_FORCE_SYSTEM "Android")

set(WN_DISABLE_CXX ON)

set(WN_PACKAGE_NAME "wn.project")