set(SRC_ANDROID
  src/Android/WNContext.h
  src/Android/WNContext_arm.h
)

source_group("src\\Android" FILES ${SRC_ANDROID})

set(ADDITIONAL_SOURCES)

list(APPEND ADDITIONAL_SOURCES ${SRC_ANDROID})

if (${WN_SYSTEM} STREQUAL "Android")
  if(${ANDROID_ARCH_NAME} STREQUAL "arm")
    list(APPEND ADDITIONAL_SOURCES asm/Android/WNGetContext_arm.S)
    list(APPEND ADDITIONAL_SOURCES asm/Android/WNSetContext_arm.S)
    if (${WN_IS_TEGRA})
      SET_SOURCE_FILES_PROPERTIES(asm/Android/WNGetContext_arm.S PROPERTIES
        LANGUAGE CXX)
      SET_SOURCE_FILES_PROPERTIES(asm/Android/WNSetContext_arm.S PROPERTIES
        LANGUAGE CXX)
      SET_SOURCE_FILES_PROPERTIES(asm/Android/WNGetContext_arm.S
        PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp")
      SET_SOURCE_FILES_PROPERTIES(asm/Android/WNSetContext_arm.S
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

add_subdirectory(test)