include(test_helper.cmake)
include_directories(${WNFramework_SOURCE_DIR}/Libraries)
include_directories(${WNFramework_BINARY_DIR}/Libraries)

foreach(overlay ${WN_ENABLED_OVERLAYS})
  include_directories(${overlay}/Libraries)
endforeach()

# These must be before all the below in order for it to work
wn_add_subdirectory(WNExecutable)
wn_add_subdirectory(WNTestUtilities)
wn_add_subdirectory(WNExecutableTest)

macro(wn_create_executable_tests_from_list)
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
      ${WNExecutableTestWrapper}
    TEST_PREFIX ${PARSED_ARGS_TEST_PREFIX}
    SOURCE_DIR ${PARSED_ARGS_SOURCE_DIR}
    SOURCES ${PARSED_ARGS_SOURCES}
    COMMON_SOURCES ${PARSED_ARGS_COMMON_SOURCES}
    RUN_WRAPPER ${PARSED_ARGS_RUN_WRAPPER}
    ADDITIONAL_INCLUDES ${PARSED_ARGS_ADDITIONAL_INCLUDES}
    LIBS WNExecutableTest ${PARSED_ARGS_LIBS}
  )
endmacro()

wn_add_subdirectory(WNContainers)
wn_add_subdirectory(WNCore)
wn_add_subdirectory(WNFileSystem)
wn_add_subdirectory(WNFunctional)
wn_add_subdirectory(WNLogging)
wn_add_subdirectory(WNMath)
wn_add_subdirectory(WNMemory)
wn_add_subdirectory(WNMultiTasking)
wn_add_subdirectory(WNNetworking)
wn_add_subdirectory(WNScripting)
wn_add_subdirectory(WNThreadSafeContainers)
wn_add_subdirectory(WNUtilities)
