include_directories(${WNFramework_SOURCE_DIR}/Libraries)
include_directories(${WNFramework_BINARY_DIR}/Libraries)
include_directories(${WNFramework_SOURCE_DIR}/Runtime)
include_directories(${WNFramework_BINARY_DIR}/Runtime)

foreach(overlay ${WN_ENABLED_OVERLAYS})
  include_directories(${overlay}/Libraries)
endforeach()

foreach(overlay ${WN_ENABLED_OVERLAYS})
  include_directories(${overlay}/Runtime)
endforeach()

wn_add_subdirectory(WNApplicationData)
wn_add_subdirectory(WNApplicationEntry)
wn_add_subdirectory(WNApplicationTest)

macro(wn_create_application_tests_from_list)
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
      ${WNFramework_SOURCE_DIR}/Runtime/cmake/application_test_wrapper.cmake
    TEST_PREFIX ${PARSED_ARGS_TEST_PREFIX}
    SOURCE_DIR ${PARSED_ARGS_SOURCE_DIR}
    SOURCES ${PARSED_ARGS_SOURCES}
    COMMON_SOURCES ${PARSED_ARGS_COMMON_SOURCES}
    RUN_WRAPPER ${PARSED_ARGS_RUN_WRAPPER}
    ADDITIONAL_INCLUDES ${PARSED_ARGS_ADDITIONAL_INCLUDES}
    LIBS WNApplicationTest ${PARSED_ARGS_LIBS}
  )
endmacro()

wn_add_subdirectory(WNGraphics)
wn_add_subdirectory(WNWindow)