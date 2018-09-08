macro(wn_add_test_wrapper OUTPUT_TEST_COMAND OUTPUT_TEST_NAME)
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
  set(${OUTPUT_TEST_NAME} ${TEST_ARGS_TEST_NAME})
  set(${OUTPUT_TEST_COMAND} ${TEST_ARGS_TEST_NAME})
endmacro()

macro(wn_post_add_test_wrapper TEST_NAME)
endmacro()