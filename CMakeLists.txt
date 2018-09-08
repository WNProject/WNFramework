project(WNFramework)
cmake_minimum_required(VERSION 2.8.12)

include(cmake/all.cmake)
include(cmake/pre_add.cmake)

add_subdirectory(Externals)
add_subdirectory(Wrappers)
add_subdirectory(Libraries)
add_subdirectory(Runtime)
add_subdirectory(Applications)
add_subdirectory(engine)

include(cmake/post_add.cmake)
