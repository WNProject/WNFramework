if(CMAKE_CONFIGURATION_TYPES)
  set(BUILD_TYPES ${CMAKE_CONFIGURATION_TYPES})
else()
  set(BUILD_TYPES ${CMAKE_BUILD_TYPE})
endif()

set_overlay_var(WN_LLVM_TARGET "X86")

wn_has_architecture(x86 HAS)

if (HAS)
  set_overlay_var(WN_LLVM_TRIPLE "i686-unknown-linux-gnu")
endif()

wn_has_architecture(x86-64 HAS)

if (HAS)
  set_overlay_var(WN_LLVM_TRIPLE "x86_64-unknown-linux-gnu")
endif()
