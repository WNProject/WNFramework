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