wn_create_executable_tests_from_list(TEST_PREFIX WNMultiTasking
  SOURCE_DIR src
  LIBS WNMultiTasking
  SOURCES
    WNFiber
    WNJobPool
    WNMutex
    WNSemaphore
    WNThreadPool
    WNThread
    WNSynchronized
)