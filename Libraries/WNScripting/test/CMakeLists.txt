
wn_create_tests_from_list(TEST_PREFIX WNScripting
  TEST_NAME WNScripting
  SOURCE_DIR src/Tests
  SOURCES
    WNASTWalker
    WNCTranslator
    WNFactory
	WNEngineJITInitializer
  LIBS WNScripting WNFileSystem
  )
