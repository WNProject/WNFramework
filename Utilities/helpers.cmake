set(WN_SYSTEM "null")
set(WN_POSIX False)
set(WN_ADDITIONAL_TEST_LIBS "")
set(WN_LLVM_EXTRA_FLAGS "")

# Check for the platform so we can include the correct files.
set_property(GLOBAL PROPERTY USE_FOLDERS On)

if (EXISTS "/opt/vc/include/bcm_host.h")
  set(WN_SYSTEM "RPI")
  set(WN_POSIX true)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")
  set(WN_ADDITIONAL_TEST_LIBS pthread)
elseif(ANDROID)
  include(${CMAKE_CURRENT_LIST_DIR}/build_apk.cmake REQUIRED)
  set(WN_SYSTEM "Android")
  set(WN_POSIX true)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}")
  set(WN_ADDITIONAL_TEST_LIBS pthread)

  if (NOT ${ANDROID_NATIVE_API_LEVEL} GREATER 13)
    message(FATAL_ERROR "Android api level must be >= 14 \"-DANDROID_NATIVE_API_LEVEL=14\"")
  endif()

  if (NOT ANDROID_SDK_DIR)
    message(WARNING  "ANDROID_SDK_DIR is not defined: .apk files will not be created.")
  endif()

  if (${ANDROID_NDK_HOST_SYSTEM_NAME} STREQUAL "windows" OR
      ${ANDROID_NDK_HOST_SYSTEM_NAME} STREQUAL "windows-x86_64")
    set(WN_NULL_LOCATION NUL)
    list(APPEND WN_LLVM_EXTRA_FLAGS
      "-DCROSS_TOOLCHAIN_FLAGS_NATIVE=-DCMAKE_C_COMPILER=cl.exe\;-DCMAKE_CXX_COMPILER=cl.exe")
  else()
    set(WN_NULL_LOCATION /dev/null)
    list(APPEND WN_LLVM_EXTRA_FLAGS
      "-DCROSS_TOOLCHAIN_FLAGS_NATIVE=-DCMAKE_C_COMPILER=cc\;-DCMAKE_CXX_COMPILER=c++")
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
  if (MSVC)
    if (MSVC_VERSION GREATER 1800)
      list(APPEND WN_LLVM_EXTRA_FLAGS "-DCMAKE_CXX_FLAGS=/Zc:sizedDealloc-")
    endif()
  endif()
  set(WN_SYSTEM "Windows")
  set(WN_POSIX false)
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
    if(WN_SYSTEM STREQUAL "Android")
      list(APPEND WN_LLVM_EXTRA_FLAGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_LIST_DIR}/wn_android_llvm.toolchain.cmake")
      set(WRAPPER_SCRIPT ${PYTHON} ${CMAKE_CURRENT_LIST_DIR}/android_env.py  ${CMAKE_TOOLCHAIN_FILE})
    else()
      list(APPEND WN_LLVM_EXTRA_FLAGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
    endif()
  else()
    list(APPEND WN_LLVM_EXTRA_FLAGS
        "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
    list(APPEND WN_LLVM_EXTRA_FLAGS
        "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
  endif()
  execute_process(
    COMMAND ${WRAPPER_SCRIPT} ${CMAKE_COMMAND}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DLLVM_BUILD_TOOLS=OFF
    -DLLVM_INCLUDE_EXAMPLES=OFF
    -DLLVM_INCLUDE_TESTS=OFF
    -DLLVM_ENABLE_ZLIB=OFF
    -DLLVM_TARGETS_TO_BUILD=host
        ${WN_LLVM_EXTRA_FLAGS}
    -G ${CMAKE_GENERATOR} ${CMAKE_SOURCE_DIR}/Externals/llvm
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Externals/llvm
    )
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
    if(ANDROID)
      set_property(TARGET ${target} PROPERTY WN_ANDROID_PERMISSIONS WRITE_EXTERNAL_STORAGE)
    endif()
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
  add_wn_executable(${PARSED_ARGS_TEST_NAME}_test SOURCES ${PARSED_ARGS_SOURCES}
    LINK_LIBRARIES gtest WNEntryPoint WNUtils ${PARSED_ARGS_LIBS})
  target_include_directories(${PARSED_ARGS_TEST_NAME}_test PRIVATE
    ${gtest_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR})

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

# Sets up the visual studio folder structure for this library,
# and any pre and post linker flags that must be used to
# link to this library.
function(add_wn_library target)
  cmake_parse_arguments(
    PARSED_ARGS
    "SHARED"
    ""
    "PRE_LINK_FLAGS;POST_LINK_FLAGS;SOURCES"
    ${ARGN})
  source_group("src" REGULAR_EXPRESSION ".*[.](c|cc|cpp|cxx)$")
  source_group("inc" REGULAR_EXPRESSION ".*[.](h|hpp)$")
  source_group("inl" REGULAR_EXPRESSION ".*[.](inl)$")
  if (PARSED_ARGS_SHARED)
    add_library(${target} SHARED ${PARSED_ARGS_SOURCES})
  else()
    add_library(${target} STATIC ${PARSED_ARGS_SOURCES})
  endif()
  set_property(TARGET ${target} PROPERTY FOLDER WNLibraries)
  if(PARSED_ARGS_PRE_LINK_FLAGS)
    set_property(TARGET ${target} PROPERTY WN_PRE_LINK_FLAGS
      ${PARSED_ARGS_PRE_LINK_FLAGS})
  endif()
  if(PARSED_ARGS_POST_LINK_FLAGS)
    set_property(TARGET ${target} PROPERTY WN_POST_LINK_FLAGS
      ${PARSED_ARGS_POST_LINK_FLAGS})
  endif()
endfunction(add_wn_library)

function(wn_target_link_libraries target)
  set(LIBS "")
  if (ANDROID)
    set(ANDROID_PERMISSIONS  "")
  endif()
  foreach(library ${ARGN})
    if (TARGET ${library})
      get_target_property(PRE_FLAGS ${library} WN_PRE_LINK_FLAGS)
      get_target_property(POST_FLAGS ${library} WN_POST_LINK_FLAGS)
      if (PRE_FLAGS)
        list(APPEND LIBS ${PRE_FLAGS})
      endif()
      list(APPEND LIBS ${library})
      if (POST_FLAGS)
        list(APPEND LIBS ${POST_FLAGS})
      endif()

      if (ANDROID)
        get_target_property(PERMS ${library} WN_ANDROID_PERMISSIONS)
        if (PERMS)
          list(APPEND ANDROID_PERMISSIONS ${PERMS})
        endif()
      endif()
    else()
      list(APPEND LIBS ${library})
    endif()
  endforeach()

  if (ANDROID)
    get_target_property(PERMS ${target} WN_ANDROID_PERMISSIONS)
    if (PERMS)
      list(APPEND PERMS ${ANDROID_PERMISSIONS})
    else()
      set(PERMS ${ANDROID_PERMISSIONS})
    endif()
    if (ANDROID_PERMISSIONS)
      set_target_properties(${target} PROPERTIES WN_ANDROID_PERMISSIONS
       "${ANDROID_PERMISSIONS}")
    endif()
  endif()
  target_link_libraries(${target} ${LIBS})
endfunction()

# Sets up the visual studio folder structure for this library.
function(add_wn_header_library target)
  LIST(LENGTH WN_BUILD_TYPES NUM_TYPES)
  # Only build a header-only library for display in
  # multi-configuration tools. (Visual studio)
  if (NOT ${NUM_TYPES} EQUAL 1)
    source_group("src" REGULAR_EXPRESSION ".*[.](c|cc|cpp|cxx)$")
    source_group("inc" REGULAR_EXPRESSION ".*[.](h|hpp)$")
    source_group("inl" REGULAR_EXPRESSION ".*[.](inl)$")
    add_wn_library(${target} SOURCES ${ARGN})
    set_property(TARGET ${target} PROPERTY FOLDER WNLibraries)
    set_property(TARGET ${target} PROPERTY LINKER_LANGUAGE CXX)
  endif()
endfunction(add_wn_header_library)

function(add_wn_executable target)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    ""
    "SOURCES;LINK_LIBRARIES"
    ${ARGN})
  if (WN_SYSTEM STREQUAL "Android")
    add_wn_library(${target} SHARED SOURCES ${PARSED_ARGS_SOURCES})
    if (PARSED_ARGS_LINK_LIBRARIES)
      wn_target_link_libraries(${target} PRIVATE ${PARSED_ARGS_LINK_LIBRARIES})
    endif()
    build_apk(ACTIVITY ${target}
      PROGRAM_NAME ${target}
      TARGET ${target})
  else()
    add_executable(${target} ${PARSED_ARGS_SOURCES})
    if (PARSED_ARGS_LINK_LIBRARIES)
      wn_target_link_libraries(${target} PRIVATE ${PARSED_ARGS_LINK_LIBRARIES})
    endif()
  endif()
endfunction()

# Sets up the visual studio folder structure for this tool.
function(add_wn_tool target)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    ""
    "SOURCES;LINK_LIBRARIES"
    ${ARGN})
  source_group("src" REGULAR_EXPRESSION ".*[.](c|cc|cpp|cxx)$")
  source_group("inc" REGULAR_EXPRESSION ".*[.](h|hpp)$")
  source_group("inl" REGULAR_EXPRESSION ".*[.](inl)$")
  add_wn_executable(${target} LINK_LIBRARIES ${PARSED_ARGS_LINK_LIBRARIES}
    SOURCES ${PARSED_ARGS_SOURCES})
  set_property(TARGET ${target} PROPERTY FOLDER WNTools)
endfunction(add_wn_tool)
