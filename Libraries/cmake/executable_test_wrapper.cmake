function(wn_add_test_wrapper)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    "TEST_NAME"
    "SOURCES;LIBS"
    ${ARGN})
  add_wn_executable(${PARSED_ARGS_TEST_NAME}
    SOURCES ${PARSED_ARGS_SOURCES}
    LIBS ${PARSED_ARGS_LIBS})
endfunction()