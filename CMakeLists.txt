
project(WNFramework)
cmake_minimum_required(VERSION 2.8.12)
# Load all helper scripts

include(cmake/all.cmake)

include(cmake/pre_add.cmake)

add_subdirectory(Externals)

add_subdirectory(Wrappers)

# Include all of our source
add_subdirectory(Libraries)

# Runtime Libraries
add_subdirectory(Runtime)


# Applications, test or otherwise
add_subdirectory(Applications)

include(cmake/post_add.cmake)
