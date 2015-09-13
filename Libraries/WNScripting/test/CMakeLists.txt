
wn_create_tests_from_list(TEST_PREFIX WNScripting
  TEST_NAME WNScripting
  COMMON_SOURCES src/Main.cpp
  SOURCE_DIR src/Tests
  SOURCES
    WNASTWalker
    WNFactory
    WNEngineJITInitializer
  LIBS WNScripting WNFileSystem
  )
