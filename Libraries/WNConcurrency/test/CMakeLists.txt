wn_create_tests_from_list(TEST_PREFIX WNConcurrency
  COMMON_SOURCES src/Main.cpp
  SOURCE_DIR src/Tests
  SOURCES
    WNMutex
    WNSemaphore
    WNThreadPool
    WNThread)
