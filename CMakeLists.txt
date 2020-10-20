cmake_policy(SET CMP0048 NEW)
cmake_minimum_required(VERSION 3.16.3)
project(WNFramework VERSION 1.0.0)

option(USE_SCCACHE "Setup for use with sccache" ${USE_SCCACHE})
if (USE_SCCACHE)
    if (NOT CMAKE_CXX_COMPILER_LAUNCHER)
        set(CMAKE_CXX_COMPILER_LAUNCHER sccache)
    endif()
endif()

include(cmake/all.cmake)
include(cmake/pre_add.cmake)

add_subdirectory(Externals)
add_subdirectory(Wrappers)
add_subdirectory(Libraries)
add_subdirectory(Runtime)
add_subdirectory(Applications)
add_subdirectory(engine)

include(cmake/post_add.cmake)
