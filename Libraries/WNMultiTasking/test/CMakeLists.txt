wn_create_executable_tests_from_list(
  TEST_PREFIX WNMultiTasking
  SOURCE_DIR src
  SOURCES
    WNFiber
    WNJobPool
    WNMutex
    WNSemaphore
    WNSharedMutex
    WNSpinLock
    WNSynchronized
    WNThreadPool
    WNThread
  LIBS
    WNMultiTasking
)
