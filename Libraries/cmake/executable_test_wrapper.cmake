macro(wn_add_test_wrapper)
  cmake_parse_arguments(
    TEST_ARGS
    ""
    "TEST_NAME"
    "SOURCES;LIBS"
    ${ARGN})
  add_wn_executable(${TEST_ARGS_TEST_NAME}
    SOURCES ${TEST_ARGS_SOURCES}
    LIBS ${TEST_ARGS_LIBS})
endmacro()