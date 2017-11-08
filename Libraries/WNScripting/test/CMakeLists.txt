wn_create_executable_tests_from_list(TEST_PREFIX WNScripting
  TEST_NAME WNScripting
  COMMON_SOURCES
    inc/Common.h
  SOURCE_DIR
    src/Tests
  SOURCES
    WNASTCodeGenerator
    WNASTWalker
    WNCTranslator
    WNFactory
    WNEngineJITInitializer
  LIBS
    WNScripting
)

wn_create_call_test(
  ScriptingFileCheck
    WORKING_DIRECTORY
      ${WNFramework_BINARY_DIR}/Applications/script_test_runner/
    COMMAND
      script_test_runner${CMAKE_EXECUTABLE_SUFFIX}
    --data_dir ${CMAKE_CURRENT_SOURCE_DIR}/filecheck
      --test_file
    UNIQUE_ARGS
      array_tests.wns
      assignment_tests.wns
      break_tests.wns
      continue_tests.wns
      do_loop_tests.wns
      empty_expression.wns
      function_call_tests.wns
      if_test.wns
      member_function_tests.wns
      relative_construction_test.wns
      simple_struct_test.wns
      scope_tests.wns
      shared_objects.wns
      struct_in_struct_tests.wns
      # TODO(awoloszyn): This test breaks JIT. Fix JIT for this test.
      #struct_with_array_tests.wns
      temporary_objects.wns
)