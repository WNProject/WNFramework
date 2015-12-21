set(SRC_ANDROID
  src/Android/WNContext.h
  src/Android/WNContext_arm.h
  src/Android/WNContext_x86.h
  src/Android/WNMakeContext.cpp
  src/Android/WNSwapContext.cpp
)

set(ADDITIONAL_SOURCES)
set(ANDROID_ASSEMBLY)

if (${WN_SYSTEM} STREQUAL "Android")
  if(${ANDROID_ARCH_NAME} STREQUAL "arm")
    list(APPEND ANDROID_ASSEMBLY asm/Android/WNGetContext_arm.S
      asm/Android/WNSetContext_arm.S)
    list(APPEND SRC_ANDROID
      src/Android/WNMakeContext_arm.cpp)
  elseif(${ANDROID_ARCH_NAME} STREQUAL "x86")
    list(APPEND ANDROID_ASSEMBLY asm/Android/WNGetContext_x86.S
      asm/Android/WNSetContext_x86.S)
    list(APPEND SRC_ANDROID
      src/Android/WNMakeContext_x86.cpp)
  endif()

  if (${WN_IS_TEGRA})
    SET_SOURCE_FILES_PROPERTIES(${ANDROID_ASSEMBLY} PROPERTIES LANGUAGE CXX)
    SET_SOURCE_FILES_PROPERTIES(${ANDROID_ASSEMBLY} PROPERTIES COMPILE_FLAGS
      "-x assembler-with-cpp")
  endif()
  source_group("src\\Android" FILES ${SRC_ANDROID})
  list(APPEND ADDITIONAL_SOURCES ${SRC_ANDROID})
  list(APPEND ADDITIONAL_SOURCES ${ANDROID_ASSEMBLY})
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