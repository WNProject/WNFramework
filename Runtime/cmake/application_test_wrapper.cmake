macro(wn_add_test_wrapper)
  cmake_parse_arguments(
    TEST_ARGS
    ""
    "TEST_NAME"
    "SOURCES;LIBS;ADDITIONAL_INCLUDES"
    ${ARGN})

  add_wn_application(${TEST_ARGS_TEST_NAME}
    SOURCES ${TEST_ARGS_SOURCES}
    LIBS ${TEST_ARGS_LIBS}
    INCLUDES ${TEST_ARGS_ADDITIONAL_INCLUDES})
endmacro()