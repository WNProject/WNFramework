wn_create_tests_from_list(TEST_PREFIX WNFileSystem
  SOURCE_DIR src
  SOURCES
    WNUtilities
  LIBS WNFileSystem
)

wn_create_tests_from_list(TEST_PREFIX WNFileSystem
  SOURCE_DIR src
  SYSTEMS_TEST
  SOURCES
    WNFactory
    WNFile
    WNMapping
  LIBS WNFileSystem
)
