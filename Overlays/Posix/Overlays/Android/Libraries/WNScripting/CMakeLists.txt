string(TOLOWER ${ANDROID_NDK_HOST_SYSTEM_NAME} ANDROID_SYSTEM_LOWER)

set_overlay_var(WN_LLVM_TRIPLE ${ANDROID_LLVM_TRIPLE})

if (${ANDROID_SYSTEM_LOWER} STREQUAL "windows" OR
  ${ANDROID_SYSTEM_LOWER} STREQUAL "windows-x86_64")
  set_overlay_var(WN_LLVM_CXX_COMPILER "cl.exe")
  set_overlay_var(WN_LLVM_C_COMPILER "cl.exe")
else()
  set_overlay_var(WN_LLVM_CXX_COMPILER "c++")
  set_overlay_var(WN_LLVM_C_COMPILER "cc")
endif()
append_to_overlay_var(WN_LLVM_EXTRA_FLAGS
  "-DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}")
append_to_overlay_var(WN_LLVM_EXTRA_FLAGS
  "-DANDROID_NDK=${ANDROID_NDK}")
append_to_overlay_var(WN_LLVM_EXTRA_FLAGS
  "-DANDROID_ABI=${ANDROID_ABI}")
append_to_overlay_var(WN_LLVM_EXTRA_FLAGS
  "-DANDROID_TOOLCHAIN_NAME=${ANDROID_TOOLCHAIN_NAME}")
append_to_overlay_var(WN_LLVM_EXTRA_FLAGS
  "-DANDROID_STL=${ANDROID_STL}")

if (NDK_CCACHE)
  append_to_overlay_var(WN_LLVM_EXTRA_FLAGS
    "-DNDK_CCACHE=${NDK_CCACHE}")
endif()

append_to_overlay_var(WN_LLVM_EXTRA_FLAGS
  "-DCMAKE_CXX_FLAGS=-D__ANDROID_NDK__")
find_host_program(PYTHON python)
append_to_overlay_var(WN_LLVM_EXTRA_FLAGS
  "-DPYTHON_EXECUTABLE=${PYTHON}")
set_overlay_var(WN_LLVM_TOOLCHAIN
  "${CMAKE_CURRENT_LIST_DIR}/build_tools/wn_android_llvm.toolchain.cmake")
append_to_overlay_var(WN_LLVM_WRAPPER
  ${PYTHON})
append_to_overlay_var(WN_LLVM_WRAPPER
  ${CMAKE_CURRENT_LIST_DIR}/build_tools/android_env.py)
string(REPLACE "\\" "/" CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE})
append_to_overlay_var(WN_LLVM_WRAPPER
  "${CMAKE_TOOLCHAIN_FILE}")

add_wn_library(WNScriptingFixes SOURCES src/Android/AndroidFixes.cpp)

overlay_library_sources(
  LIBS WNScriptingFixes)
