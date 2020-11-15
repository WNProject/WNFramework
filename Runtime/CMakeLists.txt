include(test_helper.cmake)

include_directories(${WNFramework_BINARY_DIR}/Libraries)
include_directories(${WNFramework_BINARY_DIR}/wrappers)
include_directories(${WNFramework_BINARY_DIR}/Runtime)

set(REVERSE_OVERLAYS ${WN_ENABLED_OVERLAYS})

list(REVERSE REVERSE_OVERLAYS)

foreach(overlay ${REVERSE_OVERLAYS})
  include_directories(${overlay}/Libraries)
  include_directories(${overlay}/wrappers)
  include_directories(${overlay}/Runtime)
endforeach()

include_directories(${WNFramework_SOURCE_DIR}/Libraries)
include_directories(${WNFramework_SOURCE_DIR}/wrappers)
include_directories(${WNFramework_SOURCE_DIR}/Runtime)

wn_add_subdirectory(WNApplicationData)
wn_add_subdirectory(WNApplicationEntry)
wn_add_subdirectory(WNApplicationTest)

macro(wn_create_application_tests_from_list)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    "RUN_WRAPPER;TEST_PREFIX;SOURCE_DIR"
    "SOURCES;COMMON_SOURCES;ADDITIONAL_INCLUDES;LIBS;LABELS"
    ${ARGN}
  )
  wn_create_tests_from_list_internal(
    TEST_WRAPPER ${WNApplicationTestWrapper}
    TEST_PREFIX ${PARSED_ARGS_TEST_PREFIX}
    SOURCE_DIR ${PARSED_ARGS_SOURCE_DIR}
    SOURCES ${PARSED_ARGS_SOURCES}
    COMMON_SOURCES ${PARSED_ARGS_COMMON_SOURCES}
    RUN_WRAPPER ${PARSED_ARGS_RUN_WRAPPER}
    ADDITIONAL_INCLUDES ${PARSED_ARGS_ADDITIONAL_INCLUDES}
    LIBS WNApplicationTest ${PARSED_ARGS_LIBS}
    LABELS ${PARSED_ARGS_LABELS}
  )
endmacro()

macro(wn_create_commandline_application_tests_from_list)
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
    LIBS
      WNApplicationTest
      WNApplicationEntry
      executable_entry
      ${PARSED_ARGS_LIBS}
    LABELS ${PARSED_ARGS_LABELS}
  )
endmacro()

wn_add_subdirectory(WNFileSystem)
wn_add_subdirectory(WNGraphics)
wn_add_subdirectory(networking)
wn_add_subdirectory(platform_utils)
wn_add_subdirectory(WNWindow)
