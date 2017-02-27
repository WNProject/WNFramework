# Normally we wouldn't forcibly NOT do stuff
# but this requires a lot of default android variables.
# If we are not android then these wont be set, and set_var
# and if and friends don't play nice.

if (WN_OVERLAY_IS_ENABLED AND NOT WN_DISABLE_CXX)
  string(TOLOWER ${ANDROID_NDK_HOST_SYSTEM_NAME} ANDROID_SYSTEM_LOWER)
  set_overlay_var(WN_LLVM_TRIPLE ${ANDROID_LLVM_TRIPLE})
  if(${ANDROID_SYSROOT_ABI} STREQUAL "arm")
    set_overlay_var(WN_LLVM_TARGET "ARM")
  elseif (${ANDROID_SYSROOT_ABI} STREQUAL "arm64")
    set_overlay_var(WN_LLVM_TARGET "AArch64")
  elseif(${ANDROID_SYSROOT_ABI} STREQUAL "x86")
    set_overlay_var(WN_LLVM_TARGET "X86")
  elseif(${ANDROID_SYSROOT_ABI} STREQUAL "x86_64")
    set_overlay_var(WN_LLVM_TARGET "X86")
  endif()

  append_to_overlay_var(WN_LLVM_EXTRA_FLAGS
    "-DLLVM_TABLEGEN=${WN_ANDROID_LLVM_TBLGEN}")
  
  append_to_overlay_var(WN_LLVM_EXTRA_FLAGS
    "-DCMAKE_CROSSCOMPILING=OFF")
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

  append_to_overlay_var(WN_LLVM_EXTRA_FLAGS
    "-DCMAKE_CXX_FLAGS=-D__ANDROID_NDK__")
  find_program(PYTHON python)
  
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
elseif(WN_OVERLAY_IS_ENABLED)
  append_to_overlay_var(SUB_BUILD_TARGET_DEPS llvm-host-tblgen)
  add_custom_target(llvm-host-tblgen
    WORKING_DIRECTORY
      ${WNFramework_BINARY_DIR}/host/Externals/llvm/)
  add_custom_command(TARGET llvm-host-tblgen
    COMMAND
      ${CMAKE_COMMAND} --build . --target llvm-tblgen
    WORKING_DIRECTORY
      ${WNFramework_BINARY_DIR}/host/Externals/llvm/)
endif()

overlay_library_sources(
  LIBS WNScriptingFixes)