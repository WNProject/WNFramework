if(CMAKE_CONFIGURATION_TYPES)
  set(BUILD_TYPES ${CMAKE_CONFIGURATION_TYPES})
else()
  set(BUILD_TYPES ${CMAKE_BUILD_TYPE})
endif()


wn_has_architecture(x86-64 HAS)

if (HAS)
  set_overlay_var(WN_LLVM_TARGET "X86")
  set_overlay_var(WN_LLVM_TRIPLE "x86_64-apple-darwin")
endif()

wn_has_architecture(ARM64 HAS)

if (HAS)
  set_overlay_var(WN_LLVM_TARGET "AArch64")
  set_overlay_var(WN_LLVM_TRIPLE "aarch64-apple-darwin")
endif()

