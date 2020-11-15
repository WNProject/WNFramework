macro(wn_add_test_wrapper OUTPUT_TEST_COMAND OUTPUT_TEST_NAME)
  if(NOT WN_ANDROID_WRAPPER)
    cmake_parse_arguments(
      TEST_ARGS
      ""
      "TEST_NAME"
      "SOURCES;LIBS;ADDITIONAL_INCLUDES"
      ${ARGN})

    add_wn_executable(${TEST_ARGS_TEST_NAME}
      SOURCES ${TEST_ARGS_SOURCES}
      LIBS ${TEST_ARGS_LIBS}
      INCLUDES ${TEST_ARGS_ADDITIONAL_INCLUDES})
    set(${OUTPUT_TEST_COMAND} ${TEST_ARGS_TEST_NAME})
    set(${OUTPUT_TEST_NAME} ${TEST_ARGS_TEST_NAME})
  else()
    cmake_parse_arguments(
      TEST_ARGS
      ""
      "TEST_NAME;ABI"
      "SOURCES;LIBS;ADDITIONAL_INCLUDES"
      ${ARGN})
    set(${OUTPUT_TEST_COMAND} "python;${WNFramework_BINARY_DIR}/${TEST_ARGS_ABI}/bin/${TEST_ARGS_TEST_NAME}.py;--install;--run;--stripped;--remove;--binary-dir;${WNFramework_BINARY_DIR}/${TEST_ARGS_ABI}/bin/")
    set(${OUTPUT_TEST_NAME} "${TEST_ARGS_TEST_NAME}-${TEST_ARGS_ABI}")
  endif()
endmacro()

macro(wn_post_add_test_wrapper OUTPUT_TEST_NAME)
  set_tests_properties(
    ${OUTPUT_TEST_NAME}
    PROPERTIES
      LABELS REQUIRES_HARDWARE
   )
endmacro()
