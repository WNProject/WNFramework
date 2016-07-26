wn_create_tests_from_list(TEST_PREFIX WNMultiTasking
  SOURCE_DIR src
  LIBS WNMultiTasking
  SOURCES
    WNFiber
    WNJobPool
    WNMutex
    WNSemaphore
    WNThreadPool
    WNThread
)

if (WN_PLATFORM STREQUAL "Android")
  wn_create_tests_from_list(TEST_PREFIX WNMultiTasking
    SOURCE_DIR src/Android
    LIBS WNMultiTasking
    SOURCES
      WNContextAndroid
  )
endif()