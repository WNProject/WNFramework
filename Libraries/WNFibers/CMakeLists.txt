set(INC_ANDROID
  inc/Android/context.h
)

set (ADDITIONAL_SOURCES)

if (${WN_SYSTEM} STREQUAL "Android")
  if(${ANDROID_ARCH_NAME} STREQUAL "arm")
  list(APPEND ADDITIONAL_SOURCES asm/Android/WNMakeContext_arm.S)
    if (${WN_IS_TEGRA})
      SET_SOURCE_FILES_PROPERTIES(asm/Android/WNMakeContext_arm.S PROPERTIES LANGUAGE CXX )
      SET_SOURCE_FILES_PROPERTIES(asm/Android/WNMakeContext_arm.S
        PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp")
    endif()
  elseif(${ANDROID_ARCH_NAME} STREQUAL "x86")
    list(APPEND ADDITIONAL_SOURCES asm/Android/WNMakeContext_x86.s)
    if (${WN_IS_TEGRA})
      SET_SOURCE_FILES_PROPERTIES(asm/Android/WNMakeContext_x86.S PROPERTIES LANGUAGE CXX )
      SET_SOURCE_FILES_PROPERTIES(asm/Android/WNMakeContext_x86.S
        PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp")
    endif()
  endif()
endif()

add_wn_library(WNFibers SOURCES
  inc/WNFiber.h
  src/WNFiber.cpp
  ${ADDITIONAL_SOURCES}
)

target_include_directories(WNFibers
  PUBLIC
  ${CMAKE_SOURCE_DIR}/Libraries)

source_group("inc\\Android" FILES ${INC_ANDROID})
add_subdirectory(test)