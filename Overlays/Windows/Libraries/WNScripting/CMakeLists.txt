if(CMAKE_CONFIGURATION_TYPES)
  set(BUILD_TYPES ${CMAKE_CONFIGURATION_TYPES})
else()
  set(BUILD_TYPES ${CMAKE_BUILD_TYPE})
endif()

set_overlay_var(WN_LLVM_TARGET "X86")

wn_has_architecture(x86 HAS)

if (HAS)
  set_overlay_var(WN_LLVM_TRIPLE "i686-pc-win32")
endif()

wn_has_architecture("x86-64" HAS)

if (HAS)
  set_overlay_var(WN_LLVM_TRIPLE "x86_64-pc-win32")
endif()

foreach(configuration ${BUILD_TYPES})
  if(${configuration} STREQUAL "Debug")
    append_to_overlay_var(WN_LLVM_EXTRA_FLAGS "-DLLVM_USE_CRT_DEBUG=MTd")
  elseif(${configuration} STREQUAL "Release")
    append_to_overlay_var(WN_LLVM_EXTRA_FLAGS "-DLLVM_USE_CRT_RELEASE=MT")
  elseif(${configuration} STREQUAL "MinSizeRel")
    append_to_overlay_var(WN_LLVM_EXTRA_FLAGS "-DLLVM_USE_CRT_MINSIZEREL=MT")
  elseif(${configuration} STREQUAL "RelWithDebInfo")
    append_to_overlay_var(WN_LLVM_EXTRA_FLAGS "-DLLVM_USE_CRT_RELWITHDEBINFO=MT")
  endif()
endforeach()

if(WN_LOW_RESOURCE_MODE)
  foreach(lang WN_CXX_FLAGS WN_CMAKE_C_FLAGS
              WN_CXX_FLAGS_RELEASE WN_CMAKE_C_FLAGS_RELEASE
              WN_CXX_FLAGS_DEBUG WN_CMAKE_C_FLAGS_DEBUG
              WN_CXX_FLAGS_RELWITHDEBINFO WN_CMAKE_C_FLAGS_RELWITHDEBINFO
              WN_CXX_FLAGS_MINSIZEREL WN_CMAKE_C_FLAGS_MINSIZEREL)
    string(REGEX REPLACE "/Z7" "" ${lang} "${${lang}}")
    string(REGEX REPLACE "-Z7" "" ${lang} "${${lang}}")  
    set_overlay_var(${lang} "${${lang}}")
  endforeach()
endif()
