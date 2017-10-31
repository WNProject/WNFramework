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
      empty_expression.wns
      if_test.wns
      simple_struct_test.wns
      scope_tests.wns
)