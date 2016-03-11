set(WN_SYSTEM "null")
set(WN_POSIX False)
set(WN_ADDITIONAL_TEST_LIBS "")
set(WN_LLVM_EXTRA_FLAGS "")

# Check for the platform so we can include the correct files.
set_property(GLOBAL PROPERTY USE_FOLDERS On)

list(APPEND WN_LLVM_EXTRA_FLAGS "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")

# Search packages for host system instead of packages for target system
# in case of cross compilation these macro should be defined by toolchain file
if(NOT COMMAND find_host_package)
  macro(find_host_package)
    find_package(${ARGN})
  endmacro()
endif()
if(NOT COMMAND find_host_program)
  macro(find_host_program)
    find_program(${ARGN})
  endmacro()
endif()
set(WN_IS_TEGRA OFF)
if (EXISTS "/opt/vc/include/bcm_host.h")
  set(WN_SYSTEM "RPI")
  set(WN_POSIX true)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")
  set(WN_ADDITIONAL_TEST_LIBS pthread)
elseif(ANDROID OR ${CMAKE_SYSTEM_NAME} STREQUAL "Android")
  if (CMAKE_SYSTEM_NAME STREQUAL "Android")
  # This is the case that we are building with Tegra NSight.
  # Let it handle the APK generation.
    if(NOT ANDROID)
      set(WN_IS_TEGRA ON)
      include(${CMAKE_CURRENT_LIST_DIR}/tegra_build.cmake REQUIRED)
    endif()
    set(ANDROID_NDK_HOST_SYSTEM_NAME "Windows")
    if(NOT ANDROID_NDK)
      set(ANDROID_NDK $ENV{ANDROID_NDK_ROOT})
    endif()
    if (NOT ANDROID_NDK)
      set(ANDROID_NDK $ENV{ANDROID_NDK})
    endif()
    if (NOT ANDROID_NDK)
      message(FATAL_ERROR "Set ANDROID_NDK or ANDROID_NDK_ROOT")
    endif()
    string(REPLACE "\\" "/" ANDROID_NDK "${ANDROID_NDK}")
  endif()
  if (NOT ${WN_IS_TEGRA})
    include(${CMAKE_CURRENT_LIST_DIR}/build_apk.cmake REQUIRED)
  endif()
  set(WN_SYSTEM "Android")
  set(WN_POSIX true)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}")
  set(WN_ADDITIONAL_TEST_LIBS pthread)
  string(REPLACE "-DDEBUG" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  if (${WN_IS_TEGRA})
    option(ANDROID_NATIVE_API_LEVEL "Android API Level" "14")
    add_compile_options("-DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}")
  endif()
  if (NOT ${ANDROID_NATIVE_API_LEVEL} GREATER 13)
    message(FATAL_ERROR "Android api level must be >= 14 \"-DANDROID_NATIVE_API_LEVEL=14\"")
  endif()

  if (NOT ANDROID_SDK)
    set(ANDROID_SDK "$ENV{ANDROID_SDK}")
  endif()

  if (NOT ANDROID_SDK)
    message(WARNING  "ANDROID_SDK is not defined: .apk files will not be created.")
  else()
    string(REPLACE "\\" "/" ANDROID_SDK "${ANDROID_SDK}")
  endif()
  string(TOLOWER ${ANDROID_NDK_HOST_SYSTEM_NAME} ANDROID_SYSTEM_LOWER)
  if (${ANDROID_SYSTEM_LOWER} STREQUAL "windows" OR
      ${ANDROID_SYSTEM_LOWER} STREQUAL "windows-x86_64")
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
  set(WN_SYSTEM "Windows")
  # Determine correct CRT type to use for LLVM based on configuration type
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
  # Fix for LLVM Visual Studio 2015 RC issue
  if(MSVC)
    if(MSVC_VERSION GREATER 1800)
      list(APPEND WN_LLVM_EXTRA_FLAGS "-DCMAKE_CXX_FLAGS=/Zc:sizedDealloc-")
    endif()
  endif()
  # Preprocessor definitions
  add_compile_options(/D_CRT_SECURE_NO_WARNINGS) # Disable CRT secure warnings
  add_compile_options(/D_SCL_SECURE_NO_WARNINGS) # Disable SCL secure warnings
  add_compile_options(/D_HAS_EXCEPTIONS=0) # Disable STL exceptions
  # Compiler options

  foreach(lang CMAKE_CXX_FLAGS CMAKE_C_FLAGS
               CMAKE_CXX_FLAGS_RELEASE CMAKE_C_FLAGS_RELEASE
               CMAKE_CXX_FLAGS_DEBUG CMAKE_C_FLAGS_DEBUG
               CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS_RELWITHDEBINFO
               CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_C_FLAGS_MINSIZEREL)
    # Remove any flags related to exceptions
    if(${lang} MATCHES "/EHsc")
      string(REGEX REPLACE "/EHsc" "" ${lang} "${${lang}}")
    endif()
    # Remove any flags related to rtti
    if(${lang} MATCHES "/GR")
      string(REGEX REPLACE "/GR" "" ${lang} "${${lang}}")
    endif()
    # Remove any flags related to warning level
    if(${lang} MATCHES "/W3")
      string(REGEX REPLACE "/W3" "" ${lang} "${${lang}}")
    endif()
    # Remove any flags related to dynamic crt
    if(${lang} MATCHES "/MDd")
      string(REGEX REPLACE "/MDd" "" ${lang} "${${lang}}")
    endif()
    # Remove any flags related to dynamic crt
    if(${lang} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "" ${lang} "${${lang}}")
    endif()
  endforeach()
  add_compile_options(/W4) # Adjust warnings to level 4
  add_compile_options(/WX) # Enable warnings as errors
  add_compile_options(/GR-) # Disable run-time type information
  add_compile_options(/MP) # Enable multi-processor compilation
  add_compile_options($<$<CONFIG:Debug>:/MTd> # Adjust CRT usage to static for all configs
                      $<$<NOT:$<CONFIG:Debug>>:/MT>)
  add_compile_options($<$<CONFIG:Release>:/Ox>) # Adjust optimization to full optimization
  add_compile_options($<$<CONFIG:Release>:/Ob2>) # Adjust inlining to any suitable
  add_compile_options($<$<CONFIG:Release>:/Os>) # Adjust to favour size (tend to still be better than /Ot which favours speed)
  add_compile_options($<$<CONFIG:Release>:/Oy>) # Adjust to remove frame pointers
  add_compile_options($<$<CONFIG:Release>:/Oi>) # Enable intrinsic functions
  add_compile_options($<$<CONFIG:Release>:/GT>) # Enable fiber-safe optimizations
  add_compile_options($<$<CONFIG:Release>:/GL>) # Enable whole program optimization
  add_compile_options($<$<CONFIG:Release>:/Gm->) # Disable minimal rebuild
  # Linker options
  foreach(type EXE SHARED STATIC)
    # Enable link time code generation
    if(NOT "${CMAKE_${type}_LINKER_FLAGS_RELEASE}" MATCHES "/LTCG")
      set(CMAKE_${type}_LINKER_FLAGS_RELEASE "${CMAKE_${type}_LINKER_FLAGS_RELEASE} /LTCG")
    endif()
    if(NOT "${type}" STREQUAL "STATIC")
      # Adjust to remove unreferenced code
      if("${CMAKE_${type}_LINKER_FLAGS_RELEASE}" MATCHES "/OPT:NOREF")
        string(REGEX REPLACE "/OPT:NOREF" "/OPT:REF" CMAKE_${type}_LINKER_FLAGS_RELEASE "${CMAKE_${type}_LINKER_FLAGS_RELEASE}")
      elseif(NOT "${CMAKE_${type}_LINKER_FLAGS_RELEASE}" MATCHES "/OPT:REF")
        set(CMAKE_${type}_LINKER_FLAGS_RELEASE "${CMAKE_${type}_LINKER_FLAGS_RELEASE} /OPT:REF")
      endif()
      # Adjust to enable COMDAT folding
      if("${CMAKE_${type}_LINKER_FLAGS_RELEASE}" MATCHES "/OPT:NOICF")
        string(REGEX REPLACE "/OPT:NOICF" "/OPT:ICF" CMAKE_${type}_LINKER_FLAGS_RELEASE "${CMAKE_${type}_LINKER_FLAGS_RELEASE}")
      elseif(NOT "${CMAKE_${type}_LINKER_FLAGS_RELEASE}" MATCHES "/OPT:ICF")
        set(CMAKE_${type}_LINKER_FLAGS_RELEASE "${CMAKE_${type}_LINKER_FLAGS_RELEASE} /OPT:ICF")
      endif()
    endif()
  endforeach()
endif()

option(TRACK_ALLOCATIONS "Track all memory allocations" OFF)
if (TRACK_ALLOCATIONS)
  if(WN_SYSTEM STREQUAL "Windows")
    add_compile_options("/DWN_TRACK_ALLOCATIONS")
  else()
    add_compile_options("-DWN_TRACK_ALLOCATIONS")
  endif()
endif()

if(CMAKE_GENERATOR_TOOLSET)
  list(APPEND WN_LLVM_EXTRA_FLAGS "-T\"${CMAKE_GENERATOR_TOOLSET}\"")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-tautological-compare")
endif()

if (${WN_IS_TEGRA})
  set(ANDROID_ARCH_NAME "armv7-a")
  set(WN_TEGRA_TARGET_TRIPLE "armv7a-linux-androideabi")
  if (DEFINED ANDROID_ARCH)
    if (${ANDROID_ARCH} STREQUAL "x86")
      set(WN_TEGRA_TARGET_TRIPLE "i686-none-linux-android")
      set(ANDROID_ARCH_NAME "x86")
    elseif(${ANDROID_ARCH} STREQUAL "x86_64")
      set(WN_TEGRA_TARGET_TRIPLE "x86_64-none-linux-android")
      set(ANDROID_ARCH_NAME "x86_64")
    endif()
  endif()
endif()

list(APPEND WN_LLVM_EXTRA_FLAGS "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")

# Create the llvm build directory.
execute_process(COMMAND ${CMAKE_COMMAND} -E
  make_directory ${CMAKE_CURRENT_BINARY_DIR}/Externals/llvm)
# Configure llvm only needed the first time.
if (NOT LLVM_CONFIGURED)
  message(STATUS "Configuring LLVM dependency")
  if (${WN_IS_TEGRA})
    list(APPEND WN_LLVM_EXTRA_FLAGS
      "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_LIST_DIR}/wn_tegra_llvm.toolchain.cmake")
  else()
    if (CMAKE_TOOLCHAIN_FILE)
      if(${WN_SYSTEM} STREQUAL "Android")
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
  endif()

  set(WN_ARCH "host")
  if (${WN_IS_TEGRA})
    list(APPEND WN_LLVM_EXTRA_FLAGS "-DLLVM_HOST_TRIPLE=${WN_TEGRA_TARGET_TRIPLE}")
    list(APPEND WN_LLVM_EXTRA_FLAGS "-DCMAKE_ANDROID_ARCH=${ANDROID_ARCH_NAME}")
  endif()

  execute_process(
    COMMAND ${WRAPPER_SCRIPT} ${CMAKE_COMMAND}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DLLVM_BUILD_TOOLS=OFF
    -DLLVM_INCLUDE_EXAMPLES=OFF
    -DLLVM_INCLUDE_TESTS=OFF
    -DLLVM_ENABLE_ZLIB=OFF
    -DLLVM_TARGET_ARCH=${WN_ARCH}
    -DLLVM_TARGETS_TO_BUILD=${WN_ARCH}
        ${WN_LLVM_EXTRA_FLAGS}
    -G ${CMAKE_GENERATOR} ${CMAKE_CURRENT_SOURCE_DIR}/Externals/llvm
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Externals/llvm
    )
  set(LLVM_CONFIGURED TRUE CACHE INTERNAL "llvm has been configured" FORCE)
endif()

list(LENGTH WN_BUILD_TYPES NUM_TYPES)
set(ADDITIONAL_BUILD_PARAM)
if(NOT NUM_TYPES EQUAL 1)
  list(APPEND ADDITIONAL_BUILD_PARAM "--config" ${CMAKE_CFG_INTDIR})
endif()

include(${CMAKE_CURRENT_BINARY_DIR}/Externals/llvm/share/llvm/cmake/LLVMConfig.cmake)

set(WNScriptingLLVMComponents codegen instrumentation objcarcopts support mcjit ipo native)
add_custom_target(llvm_target
    COMMAND ${CMAKE_COMMAND} --build . ${ADDITIONAL_BUILD_PARAM}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Externals/llvm)

llvm_map_components_to_libnames(WNScriptingLLVMLibs ${WNScriptingLLVMComponents})

option(WN_BUILD_LLVM_MANUALLY
  "Do not automatically build llvm when a dependency is built"
  ${WN_BUILD_LLVM_MANUALLY})

if (NOT ${WN_BUILD_LLVM_MANUALLY})
  foreach (llvm_lib ${WNScriptingLLVMLibs})
    add_dependencies(${llvm_lib} llvm_target)
  endforeach()
endif()

get_property(OLD_DIRECTORY TARGET llvm_target PROPERTY FOLDER)
set_property(TARGET llvm_target PROPERTY FOLDER Externals/${OLD_DIRECTORY})

add_subdirectory(Externals/googletest)
enable_testing()

set(GTEST_TARGETS
    gtest
    gtest_main
    gmock
    gmock_main)

# Sets up the visual studio folder structure for gtest.
foreach (target ${GTEST_TARGETS})
    get_property(OLD_DIRECTORY TARGET ${target} PROPERTY FOLDER)
    set_property(TARGET ${target} PROPERTY FOLDER Externals/googletest/${OLD_DIRECTORY})

    if (ANDROID)
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
  add_wn_executable(${PARSED_ARGS_TEST_NAME}_test SOURCES ${PARSED_ARGS_SOURCES}
    LINK_LIBRARIES WNTesting WNUtilities ${PARSED_ARGS_LIBS})
  target_include_directories(${PARSED_ARGS_TEST_NAME}_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})

  if (ANDROID)
    find_host_program(PYTHON python)
    add_test(${PARSED_ARGS_TEST_NAME}
      ${PYTHON} ${CMAKE_CURRENT_BINARY_DIR}/${PARSED_ARGS_TEST_NAME}_test/${PARSED_ARGS_TEST_NAME}_test.py
      --install --run --remove)
  else()
    if (PARSED_ARGS_RUN_WRAPPER)
      add_test(${PARSED_ARGS_TEST_NAME}
        ${PARSED_ARGS_RUN_WRAPPER} ${PARSED_ARGS_TEST_NAME}_test)
    else()
      add_test(${PARSED_ARGS_TEST_NAME}
        ${PARSED_ARGS_TEST_NAME}_test)
    endif()
  endif()
  set_property(TARGET ${PARSED_ARGS_TEST_NAME}_test PROPERTY FOLDER
    Tests/${PARSED_ARGS_TEST_PREFIX})
endfunction()

function(wn_set_source_groups)
  foreach(ARG ${ARGN})
    string(REGEX MATCH ".*[/]" SOURCE_PATH ${ARG})
    string(REPLACE ${CMAKE_CURRENT_BINARY_DIR} "" SOURCE_PATH ${SOURCE_PATH})
    string(REPLACE "/" "\\" SOURCE_PATH ${SOURCE_PATH})
    source_group("${SOURCE_PATH}" FILES ${ARG})
  endforeach()
endfunction()

# Arguments
#     SOURCE_DIR: Directory that contains each individual test.
#     COMMON_SOURCES: Sourcs to include into each test.
#     SOURCES: List of source files in SOURCE_DIR of the form ${SOURCE}Tests.cpp.
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
    wn_create_test(TEST_NAME ${PARSED_ARGS_TEST_PREFIX}_${source}
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
    "SHARED;OBJECT"
    ""
    "PRE_LINK_FLAGS;POST_LINK_FLAGS;SOURCES"
    ${ARGN})
  wn_set_source_groups(${PARSED_ARGS_SOURCES})

  if (PARSED_ARGS_SHARED)
    add_library(${target} SHARED ${PARSED_ARGS_SOURCES})
  elseif(PARSED_ARGS_OBJECT)
    add_library(${target} OBJECT ${PARSED_ARGS_SOURCES})
  else()
    add_library(${target} STATIC ${PARSED_ARGS_SOURCES})
  endif()

  target_include_directories(${target} PUBLIC
    ${WNFramework_SOURCE_DIR}/Libraries
  )

  target_include_directories(${target} PUBLIC
    ${WNFramework_BINARY_DIR}/Libraries
  )

  set_property(TARGET ${target} PROPERTY FOLDER Libraries)

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
    add_wn_library(${target} SOURCES ${ARGN})
    set_property(TARGET ${target} PROPERTY FOLDER Libraries)
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
  if (WN_SYSTEM STREQUAL "Android" AND NOT ${WN_IS_TEGRA})
    add_wn_library(${target} SHARED SOURCES ${PARSED_ARGS_SOURCES})
  else()
    wn_set_source_groups(${PARSED_ARGS_SOURCES})
    add_executable(${target} ${PARSED_ARGS_SOURCES})
  endif()

  if (PARSED_ARGS_LINK_LIBRARIES)
    wn_target_link_libraries(${target} PRIVATE ${PARSED_ARGS_LINK_LIBRARIES})
  endif()
  if (WN_SYSTEM STREQUAL "Android")
    build_apk(ACTIVITY ${target} PROGRAM_NAME ${target} TARGET ${target})
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
  add_wn_executable(${target} LINK_LIBRARIES ${PARSED_ARGS_LINK_LIBRARIES}
    SOURCES ${PARSED_ARGS_SOURCES})
  set_property(TARGET ${target} PROPERTY FOLDER Tools)
endfunction(add_wn_tool)
