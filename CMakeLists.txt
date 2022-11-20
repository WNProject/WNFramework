if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif()
cmake_minimum_required(VERSION 3.16.3)
project(WNFramework)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
option(WN_USE_SCCACHE "Setup for use with sccache" ${WN_USE_SCCACHE})
option(
    WN_LOW_RESOURCE_MODE
    "Turn on low resource related options"
    ${WN_LOW_RESOURCE_MODE}
)

if (NOT WN_TRACY_CALLBACK_DEPTH)
    set(WN_TRACY_CALLBACK_DEPTH 0)
endif()

option(WN_ENABLE_TRACY "Enable Tracy Profiler" ${WN_ENABLE_TRACY})
set(
    WN_TRACY_CALLBACK_DEPTH ${WN_TRACY_CALLBACK_DEPTH}
    CACHE STRING "Depth of the callstack to store for tracy"
)

if (WN_USE_SCCACHE)
    if (NOT CMAKE_C_COMPILER_LAUNCHER)
        set(CMAKE_C_COMPILER_LAUNCHER sccache)
    endif()

    if (NOT CMAKE_CXX_COMPILER_LAUNCHER)
        set(CMAKE_CXX_COMPILER_LAUNCHER sccache)
    endif()
endif()

include(cmake/all.cmake)
include(cmake/pre_add.cmake)

add_subdirectory(externals)
add_subdirectory(wrappers)
add_subdirectory(Libraries)
add_subdirectory(Runtime)
add_subdirectory(applications)
add_subdirectory(engine)

include(cmake/post_add.cmake)
