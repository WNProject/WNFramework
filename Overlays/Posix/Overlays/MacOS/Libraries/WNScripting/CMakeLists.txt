if(CMAKE_CONFIGURATION_TYPES)
  set(BUILD_TYPES ${CMAKE_CONFIGURATION_TYPES})
else()
  set(BUILD_TYPES ${CMAKE_BUILD_TYPE})
endif()

set_overlay_var(WN_LLVM_TARGET "X86")

set_overlay_var(WN_LLVM_TRIPLE "x86_64-apple-darwin")
