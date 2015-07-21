cmake_minimum_required(VERSION 2.8.10)

set(WN_PREBUILD_DEPS OFF CACHE BOOL
	"Compile all dependencies during the configure step, if false it must be done manually")

project(WNFramework)

set(WN_BUILD_TYPES)
if(NOT CMAKE_CONFIGURATION_TYPES)
  if(NOT CMAKE_BUILD_TYPE OR
      ((NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug") AND
       (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release") AND
       (NOT ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel") AND
       (NOT ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")))
     message(FATAL_ERROR "You must specify a valid build type.\n"
                "-DCMAKE_BUILD_TYPE={Debug|Release|MinSizeRel|RelWithDebInfo}")
   endif()
   set(WN_BUILD_TYPES ${CMAKE_BUILD_TYPE})
else()
   set(WN_BUILD_TYPES ${CMAKE_CONFIGURATION_TYPES})
endif()

# This sets up WN variables (posix/windows etc) and
# configures/compiles llvm.
include(Utilities/helpers.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(Libraries)
add_subdirectory(Tools)

