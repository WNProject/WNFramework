wn_create_tests_from_list(TEST_PREFIX WNScripting
  TEST_NAME WNScripting
  SOURCE_DIR src/Tests
  SOURCES
    WNASTCodeGenerator
    WNASTWalker
    WNCTranslator
    WNFactory
    WNEngineJITInitializer
  LIBS WNScripting
)
