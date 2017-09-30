wn_create_executable_tests_from_list(TEST_PREFIX WNFileSystem
  SOURCE_DIR src
  SYSTEMS_TEST
  COMMON_SOURCES
    src/TestFiles.h
    src/TestFiles.cpp
  SOURCES
    WNFactory
    WNFile
    WNMapping
    WNUtilities
  LIBS
    WNFileSystem
)
