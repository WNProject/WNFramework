# Toolchain for building Android APK in WNFramework

if (DEFINED CMAKE_CROSSCOMPILING)
    # subsequent toolchain loading is not really needed
    return()
endif()

set(WN_ANDROID_SDK ""
    CACHE STRING "Location of the Android SDK")

if (WN_ANDROID_SDK)
    string(REPLACE "\\\\" "/" WN_ANDROID_SDK ${WN_ANDROID_SDK})
    string(REPLACE "\\" "/" WN_ANDROID_SDK ${WN_ANDROID_SDK})
endif()

set(WN_ANDROID_NDK "${WN_ANDROID_SDK}/ndk-bundle"
    CACHE STRING "Location of the Android NDK")
# New Versions of the NDK have a new legacy toolchain that
# doesn't work properly on windows, as it reads as MINGW.
if (EXISTS "${WN_ANDROID_NDK}/build/cmake/android-legacy.toolchain.cmake")
set(WN_ANDROID_TOOLCHAIN_FILE
    "${WN_ANDROID_NDK}/build/cmake/android-legacy.toolchain.cmake"
    CACHE STRING "Location of the android.cmake.toolchain for the NDK")
else()
set(WN_ANDROID_TOOLCHAIN_FILE
    "${WN_ANDROID_NDK}/build/cmake/android.toolchain.cmake"
    CACHE STRING "Location of the android.cmake.toolchain for the NDK")
endif()

file(TO_CMAKE_PATH ${WN_ANDROID_NDK} ANDROID_NDK)
set(WN_ANDROID_NATIVE_API_LEVEL 22
    CACHE STRING "Native API level to build")
set(WN_ANDROID_ABIS "arm;x86;arm64" CACHE STRING
    "Which Android ABIs to build")
set(WN_SANITIZED_ABIS)

foreach(ABI ${WN_ANDROID_ABIS})
    string(TOLOWER ${ABI} ABI)

    if (${ABI} MATCHES "^(android-arm|arm)(|32|eabi-v7a|-v7a|-v7)$")
        list(APPEND WN_SANITIZED_ABIS "armeabi-v7a")
    elseif(${ABI} MATCHES "^(android-arm|arm)64(|-v8a|-v8)$")
        list(APPEND WN_SANITIZED_ABIS "arm64-v8a")
    elseif(${ABI} MATCHES "^(android-|)x86$")
        list(APPEND WN_SANITIZED_ABIS "x86")
    else()
        message(FATAL_ERROR "Unrecognized Android ABI: ${ABI}")
    endif()
endforeach()

set(WN_FORCED_ARCHITECTURES ${WN_SANITIZED_ABIS})

message(STATUS "Using Android ABI: ${WN_SANITIZED_ABIS}")
message(STATUS "Using Android SDK: ${WN_ANDROID_SDK}")
message(STATUS "Using Android SDK version: ${WN_ANDROID_NATIVE_API_LEVEL}")
message(STATUS "Using Android NDK: ${WN_ANDROID_NDK}")
message(STATUS "Using Android NDK version: ${WN_ANDROID_NATIVE_API_LEVEL}")
message(STATUS "Using Android toolchain ${WN_ANDROID_TOOLCHAIN_FILE}")

set(WN_ANDROID_WRAPPER ON)
set(WN_FORCE_SYSTEM "Android")
set(WN_DISABLE_CXX ON)

set(WN_PACKAGE_DOMAIN "org;wnproject")
set(WN_PACKAGE_DOMAIN_STRING)
list(JOIN WN_PACKAGE_DOMAIN "." WN_PACKAGE_DOMAIN_STRING)