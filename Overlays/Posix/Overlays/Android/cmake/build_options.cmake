
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}")

# -DDEBUG conflicts with some libraries, remove it here
string(REPLACE "-DDEBUG" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-keep-memory")

add_compile_options(-D_WN_ANDROID)
add_compile_options(-D_XOPEN_SOURCE=600)
add_compile_options(-Wno-unused-lambda-capture)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  # Clang on android compiles with -Oz, this breaks the build in
  # subtle ways.
  add_compile_options("-Os")
endif()

if (NOT WN_ANDROID_SDK)
  message(STATUS "Warning WN_ANDROID_SDK not defined, applications will not"
    " be built")
endif()

find_program(WN_PYTHON Python)
