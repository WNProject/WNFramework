
project(WNFramework)
cmake_minimum_required(VERSION 2.8.12)
# Load all helper scripts

include(cmake/all.cmake)

include(cmake/pre_add.cmake)

# Include all of our source
add_subdirectory(Libraries)

# Runtime Libraries
add_subdirectory(Runtime)

include(cmake/post_add.cmake)