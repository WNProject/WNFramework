wn_filesystem_files(external_test_assets
  OUTPUT_DIR
    inc
  SOURCES
    src/Tests/ExternalTest/external_test.wns)

wn_create_executable_tests_from_list(TEST_PREFIX WNScripting
  TEST_NAME WNScripting
  COMMON_SOURCES
  SOURCE_DIR
    src/Tests
  SOURCES
    WNFactory
  LIBS
    WNScripting
)

wn_create_executable_tests_from_list(TEST_PREFIX WNScripting
  TEST_NAME WNScripting
  COMMON_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/inc/external_test_assets.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/external_test_assets.cpp
  SOURCE_DIR
    src/Tests
  SOURCES
    external_cpp_
  LIBS
    WNScripting
    effcee
)

wn_create_call_test(
  ScriptingFileCheck
    WORKING_DIRECTORY
      ${WNFramework_BINARY_DIR}/Applications/script_test_runner/
    APPLICATION
      script_test_runner
    COMMAND_PRE_DIR
      --data_dir
    COMMAND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/filecheck
    COMMAND_POST_DIR
      --test_file
    UNIQUE_ARGS
      array_tests.wns
      assignment_tests.wns
      constants_test.wns
      empty_expression.wns
      empty_struct.wns
      external_function_tests.wns
      function_call_tests.wns
      loop_with_break.wns
      loop_with_continue.wns
      loop_with_return.wns
      partial_for_loops.wns
      simple_functions.wns
      simple_inheritance.wns
      simple_loop_tests.wns
      simple_member_functions.wns
      simple_shared.wns
      simple_struct_test.wns
      simple_virtual.wns
      struct_assign.wns
)