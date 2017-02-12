include_directories(${WNFramework_SOURCE_DIR}/Libraries)
include_directories(${WNFramework_BINARY_DIR}/Libraries)

foreach(overlay ${WN_ENABLED_OVERLAYS})
  include_directories(${overlay}/Libraries)
endforeach()

# These must be before all the below in order for it to work
add_subdirectory(WNExecutable)
add_subdirectory(WNTestUtilities)
add_subdirectory(WNExecutableTest)

function(wn_create_executable_tests_from_list)
  cmake_parse_arguments(
    PARSED_ARGS
    "SYSTEMS_TEST"
    "RUN_WRAPPER;TEST_PREFIX;SOURCE_DIR"
    "SOURCES;COMMON_SOURCES;ADDITIONAL_INCLUDES;LIBS"
    ${ARGN}
  )
  set(ST)
  if (PARSED_ARGS_SYSTEMS_TEST)
    set(ST "SYSTEMS_TEST")
  endif()
  wn_create_tests_from_list_internal(
    ${ST}
    TEST_WRAPPER
      ${WNFramework_SOURCE_DIR}/Libraries/cmake/executable_test_wrapper.cmake
    TEST_PREFIX ${PARSED_ARGS_TEST_PREFIX}
    SOURCE_DIR ${PARSED_ARGS_SOURCE_DIR}
    SOURCES ${PARSED_ARGS_SOURCES}
    COMMON_SOURCES ${PARSED_ARGS_COMMON_SOURCES}
    RUN_WRAPPER ${PARSED_ARGS_RUN_WRAPPER}
    ADDITIONAL_INCLUDES ${PARSED_ARGS_ADDITIONAL_INCLUDES}
    LIBS WNExecutableTest ${PARSED_ARGS_LIBS}
  )
endfunction()

add_subdirectory(WNContainers)
add_subdirectory(WNCore)
add_subdirectory(WNFileSystem)
add_subdirectory(WNFunctional)
add_subdirectory(WNLogging)
add_subdirectory(WNMath)
add_subdirectory(WNMemory)
add_subdirectory(WNMultiTasking)
add_subdirectory(WNNetworking)
add_subdirectory(WNScripting)
add_subdirectory(WNThreadSafeContainers)
add_subdirectory(WNUtilities)