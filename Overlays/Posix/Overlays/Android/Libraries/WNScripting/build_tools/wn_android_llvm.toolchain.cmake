set(BASE_TOOLCHAIN_FILE $ENV{ANDROID_TOOLCHAIN_FILE} CACHE STRING "The toolchain file to use")

include(${BASE_TOOLCHAIN_FILE})

add_compile_options("-fvisibility=hidden")
add_compile_options("-Wno-unused-lambda-capture")

set( CMAKE_CXX_FLAGS_DEBUG     "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "c++ Debug flags" FORCE)
set( CMAKE_C_FLAGS_DEBUG       "${CMAKE_C_FLAGS_DEBUG}" CACHE STRING "c Debug flags" FORCE)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  # Clang on android compiles with -Oz, this breaks the build in
  # subtle ways.
  add_compile_options("-Os")
endif()
if (${WN_LOW_RESOURCE_MODE})
    add_compile_options("-g0")
endif()

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_LIST_DIR}")
