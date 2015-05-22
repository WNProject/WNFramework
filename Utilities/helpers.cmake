set(WN_SYSTEM "null")
set(WN_POSIX False)
set(WN_ADDITIONAL_TEST_LIBS "")
set(WN_ARCH "null")
set(WN_LLVM_EXTRA_FLAGS "")

# Check for the platform so we can include the correct files.
set_property(GLOBAL PROPERTY USE_FOLDERS On)

if (EXISTS "/opt/vc/include/bcm_host.h")
  set(WN_SYSTEM "RPI")
  set(WN_POSIX true)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")
  set(WN_ADDITIONAL_TEST_LIBS pthread)
  set(WN_ARCH "ARM")
elseif(ANDROID)
  set(WN_SYSTEM "Android")
  set(WN_POSIX true)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")
  set(WN_ADDITIONAL_TEST_LIBS pthread)
  string(TOUPPER ${ANDROID_ARCH_NAME} WN_ARCH)
  if (!${ANDROID_NDK_HOST_SYSTEM_NAME} STREQUAL "windows")
	if (CMAKE_C_COMPILER AND CMAKE_CXX_COMPILER)
		list(APPEND WN_LLVM_EXTRA_FLAGS
		"-DCROSS_TOOLCHAIN_FLAGS_NATIVE=-DCMAKE_C_COMPILER=gcc\;-DCMAKE_CXX_COMPILER=g++")
	endif()
  endif()
  list(APPEND WN_LLVM_EXTRA_FLAGS
    "-DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}")
  list(APPEND WN_LLVM_EXTRA_FLAGS
    "-DANDROID_NDK=${ANDROID_NDK}")
  list(APPEND WN_LLVM_EXTRA_FLAGS
    "-DANDROID_ABI=${ANDROID_ABI}")
  list(APPEND WN_LLVM_EXTRA_FLAGS
    "-DANDROID_TOOLCHAIN_NAME=${ANDROID_TOOLCHAIN_NAME}")
  list(APPEND WN_LLVM_EXTRA_FLAGS
    "-DANDROID_STL=${ANDROID_STL}")
  if (NDK_CCACHE)
    list(APPEND WN_LLVM_EXTRA_FLAGS
      "-DNDK_CCACHE=${NDK_CCACHE}")
  endif()
  list(APPEND WN_LLVM_EXTRA_FLAGS
    "-DCMAKE_CXX_FLAGS=-D__ANDROID_NDK__")
  find_host_program(PYTHON python)
  list(APPEND WN_LLVM_EXTRA_FLAGS
    "-DPYTHON_EXECUTABLE=${PYTHON}")
  list(APPEND  WN_LLVM_EXTRA_FLAGS "-DANDROID_NO_UNDEFINED=OFF")
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
  set(WN_SYSTEM "Linux")
  set(WN_POSIX true)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")
  set(WN_ADDITIONAL_TEST_LIBS pthread)
#TODO: Make this work on non X86 systems.
  set(WN_ARCH "X86")
elseif (CMAKE_SYSTEM_NAME STREQUAL Windows)
  # Determine correct CRT type to use based on configuration type
  foreach(configuration ${WN_BUILD_TYPES})
    if(${configuration} STREQUAL "Debug")
      list(APPEND WN_LLVM_EXTRA_FLAGS "-DLLVM_USE_CRT_DEBUG=MTd")
    elseif(${configuration} STREQUAL "Release")
      list(APPEND WN_LLVM_EXTRA_FLAGS "-DLLVM_USE_CRT_RELEASE=MT")
    elseif(${configuration} STREQUAL "MinSizeRel")
      list(APPEND WN_LLVM_EXTRA_FLAGS "-DLLVM_USE_CRT_MINSIZEREL=MT")
    elseif(${configuration} STREQUAL "RelWithDebInfo")
      list(APPEND WN_LLVM_EXTRA_FLAGS "-DLLVM_USE_CRT_RELWITHDEBINFO=MT")
    endif()
  endforeach()
  set(WN_SYSTEM "Windows")
  set(WN_POSIX false)
  set(WN_ARCH "X86")
  foreach(flag_var
      CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
      CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
  endforeach(flag_var)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
endif()

if(CMAKE_GENERATOR_TOOLSET)
  list(APPEND WN_LLVM_EXTRA_FLAGS "-T\"${CMAKE_GENERATOR_TOOLSET}\"")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-tautological-compare")
endif()

# Create the llvm build directory.
execute_process(COMMAND ${CMAKE_COMMAND} -E
  make_directory ${CMAKE_BINARY_DIR}/Externals/llvm)
# Configure llvm only needed the first time.
if (NOT LLVM_CONFIGURED)
  message(STATUS "Configuring LLVM dependency")
  if (CMAKE_TOOLCHAIN_FILE)
    list(APPEND WN_LLVM_EXTRA_FLAGS
      "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
  else()
    list(APPEND WN_LLVM_EXTRA_FLAGS
        "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
    list(APPEND WN_LLVM_EXTRA_FLAGS
        "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DLLVM_BUILD_TOOLS=OFF
    -DLLVM_INCLUDE_EXAMPLES=OFF
    -DLLVM_INCLUDE_TESTS=OFF
    -DLLVM_TARGETS_TO_BUILD=${WN_ARCH}
        ${WN_LLVM_EXTRA_FLAGS}
    -G ${CMAKE_GENERATOR} ${CMAKE_SOURCE_DIR}/Externals/llvm
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Externals/llvm
    OUTPUT_QUIET)
  set(LLVM_CONFIGURED TRUE CACHE INTERNAL "llvm has been configured" FORCE)
endif()
if (${WN_PREBUILD_DEPS})
  LIST(LENGTH WN_BUILD_TYPES NUM_TYPES)
  foreach(configuration ${WN_BUILD_TYPES})
          message(STATUS "Compiling LLVM Dependency ${configuration}. This could take a while")
          set(ADDITIONAL_BUILD_PARAM)
          if(NOT NUM_TYPES EQUAL 1)
            list(APPEND ADDITIONAL_BUILD_PARAM "--config" ${configuration})
          endif()
          execute_process(COMMAND ${CMAKE_COMMAND}
            --build .
            ${ADDITIONAL_BUILD_PARAM}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Externals/llvm
            OUTPUT_QUIET)
  endforeach()
endif()
include(${CMAKE_BINARY_DIR}/Externals/llvm/share/llvm/cmake/LLVMConfig.cmake)

add_subdirectory(Externals/gtest)
enable_testing()

set(GTEST_TARGETS
    gtest
    gtest_main)
# Sets up the visual studio folder structure for gtest.
foreach(target ${GTEST_TARGETS})
    get_property(OLD_DIRECTORY TARGET ${target} PROPERTY FOLDER)
    set_property(TARGET ${target} PROPERTY FOLDER Externals/gtest/${OLD_DIRECTORY})
endforeach()

# Arguments
#     TEST_NAME: Name of the test to create.
#     RUN_WRAPPER: Wrapper script to run the test.
#     TEST_PREFIX: The prefix to use for the name of the test.
#     SOURCES: Source files for this test.
#     ADDITIONAL_INCLUDES: Any additional include directories.
#     LIBS: Any additional libraries to include.
function(wn_create_test)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    "TEST_NAME;RUN_WRAPPER;TEST_PREFIX"
    "SOURCES;ADDITIONAL_INCLUDES;LIBS"
    ${ARGN})
  if(NOT PARSED_ARGS_TEST_NAME)
    message(FATAL_ERROR "You must provide a test name")
  endif()
  if(NOT PARSED_ARGS_SOURCES)
    message(FATAL_ERROR "You must provide at least one source file")
  endif()
  source_group("src" REGULAR_EXPRESSION ".*[.](c|cc|cpp|cxx)$")
  source_group("inc" REGULAR_EXPRESSION ".*[.](h|hpp)$")
  source_group("inl" REGULAR_EXPRESSION ".*[.](inl)$")
  add_executable(${PARSED_ARGS_TEST_NAME}_test ${PARSED_ARGS_SOURCES})
  target_include_directories(${PARSED_ARGS_TEST_NAME}_test PRIVATE
    ${gtest_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR})
  target_link_libraries(${PARSED_ARGS_TEST_NAME}_test
    gtest WNEntryPoint WNUtils ${PARSED_ARGS_LIBS})
  if (PARSED_ARGS_RUN_WRAPPER)
    add_test(${PARSED_ARGS_TEST_PREFIX}.${PARSED_ARGS_TEST_NAME}
      ${PARSED_ARGS_RUN_WRAPPER} ${PARSED_ARGS_TEST_NAME}_test)
  else()
    add_test(${PARSED_ARGS_TEST_PREFIX}.${PARSED_ARGS_TEST_NAME}
      ${PARSED_ARGS_TEST_NAME}_test)
  endif()
  set_property(TARGET ${PARSED_ARGS_TEST_NAME}_test PROPERTY FOLDER
    WNTests/${PARSED_ARGS_TEST_PREFIX})
endfunction()

# Arguments
#     SOURCE_DIR: Directory that contains each individual test.
#     COMMON_SOURCES: Sourcs to include into each test.
#     SOURCES: List of source files in SOURCE_DIR of th form ${SOURCE}Tests.cpp.
#     ADDITIONAL_INCLUDES: Any additional include directories.
#     RUN_WRAPPER: Wrappe script to run the test.
#     TEST_PREFIX: The prefix to use for the name of the test.
#     LIBS: Any additional libraries to include.
function(wn_create_tests_from_list)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    "RUN_WRAPPER;TEST_PREFIX;SOURCE_DIR"
    "SOURCES;COMMON_SOURCES;ADDITIONAL_INCLUDES;LIBS"
    ${ARGN})
  if(NOT PARSED_ARGS_SOURCES)
    message(FATAL_ERROR "You must provide at least one source file")
  endif()
  foreach(source ${PARSED_ARGS_SOURCES})
    wn_create_test(TEST_NAME ${source}
      TEST_PREFIX ${PARSED_ARGS_TEST_PREFIX}
      SOURCES ${PARSED_ARGS_SOURCE_DIR}/${source}Tests.cpp
         ${PARSED_ARGS_COMMON_SOURCES}
        RUN_WRAPPER ${PARSED_ARGS_RUN_WRAPPER}
        ADDITIONAL_INCLUDES ${PARSED_ARGS_ADDITIONAL_INCLUDES}
        LIBS ${PARSED_ARGS_LIBS})
  endforeach()
endfunction()

# Sets up the visual studio folder structure for this library.
function(add_wn_library target)
  source_group("src" REGULAR_EXPRESSION ".*[.](c|cc|cpp|cxx)$")
  source_group("inc" REGULAR_EXPRESSION ".*[.](h|hpp)$")
  source_group("inl" REGULAR_EXPRESSION ".*[.](inl)$")
  add_library(${target} ${ARGN})
  set_property(TARGET ${target} PROPERTY FOLDER WNLibraries)
endfunction(add_wn_library)

# Sets up the visual studio folder structure for this library.
function(add_wn_header_library target)
  LIST(LENGTH WN_BUILD_TYPES NUM_TYPES)
  # Only build a header-only library for display in
  # multi-configuration tools. (Visual studio)
  if (NOT ${NUM_TYPES} EQUAL 1)
    source_group("src" REGULAR_EXPRESSION ".*[.](c|cc|cpp|cxx)$")
    source_group("inc" REGULAR_EXPRESSION ".*[.](h|hpp)$")
    source_group("inl" REGULAR_EXPRESSION ".*[.](inl)$")
    add_library(${target} STATIC ${ARGN})
    set_property(TARGET ${target} PROPERTY FOLDER WNLibraries)
    set_property(TARGET ${target} PROPERTY LINKER_LANGUAGE CXX)
  endif()
endfunction(add_wn_header_library)

# Sets up the visual studio folder structure for this tool.
function(add_wn_tool target)
  source_group("src" REGULAR_EXPRESSION ".*[.](c|cc|cpp|cxx)$")
  source_group("inc" REGULAR_EXPRESSION ".*[.](h|hpp)$")
  source_group("inl" REGULAR_EXPRESSION ".*[.](inl)$")
  add_executable(${target} ${ARGN})
  set_property(TARGET ${target} PROPERTY FOLDER WNTools)
endfunction(add_wn_tool)
