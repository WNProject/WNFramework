include(test_helper.cmake)

include_directories(${WNFramework_BINARY_DIR}/Libraries)
include_directories(${WNFramework_BINARY_DIR}/wrappers)

set(REVERSE_OVERLAYS ${WN_ENABLED_OVERLAYS})

list(REVERSE REVERSE_OVERLAYS)

foreach(overlay ${REVERSE_OVERLAYS})
  include_directories(${overlay}/Libraries)
  include_directories(${overlay}/wrappers)
endforeach()

include_directories(${WNFramework_SOURCE_DIR}/Libraries)
include_directories(${WNFramework_SOURCE_DIR}/wrappers)

# These must be before all the below in order for it to work
wn_add_subdirectory(executable_data)
wn_add_subdirectory(executable_entry)
wn_add_subdirectory(WNTestUtilities)
wn_add_subdirectory(executable_test)

macro(wn_create_executable_tests_from_list)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    "RUN_WRAPPER;TEST_PREFIX;SOURCE_DIR"
    "SOURCES;COMMON_SOURCES;ADDITIONAL_INCLUDES;LIBS;LABELS"
    ${ARGN}
  )

  wn_create_tests_from_list_internal(
    TEST_WRAPPER ${WNExecutableTestWrapper}
    TEST_PREFIX ${PARSED_ARGS_TEST_PREFIX}
    SOURCE_DIR ${PARSED_ARGS_SOURCE_DIR}
    SOURCES ${PARSED_ARGS_SOURCES}
    COMMON_SOURCES ${PARSED_ARGS_COMMON_SOURCES}
    RUN_WRAPPER ${PARSED_ARGS_RUN_WRAPPER}
    ADDITIONAL_INCLUDES ${PARSED_ARGS_ADDITIONAL_INCLUDES}
    LIBS executable_test ${PARSED_ARGS_LIBS}
    LABELS ${PARSED_ARGS_LABELS}
  )
endmacro()

wn_add_subdirectory(WNContainers)
wn_add_subdirectory(WNCore)
wn_add_subdirectory(WNFileSystem)
wn_add_subdirectory(WNFunctional)
wn_add_subdirectory(WNLogging)
wn_add_subdirectory(hashing)
wn_add_subdirectory(WNMath)
wn_add_subdirectory(WNMemory)
wn_add_subdirectory(WNMultiTasking)
wn_add_subdirectory(WNNetworking)
wn_add_subdirectory(WNScripting)
wn_add_subdirectory(WNThreadSafeContainers)
wn_add_subdirectory(WNUtilities)
