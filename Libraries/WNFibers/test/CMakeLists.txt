wn_create_tests_from_list(TEST_PREFIX WNFibers
  SOURCE_DIR src/
  LIBS WNFibers
  SOURCES
    WNFiber
)

if (${WN_SYSTEM} STREQUAL "Android")
  wn_create_tests_from_list(TEST_PREFIX WNFibers
    SOURCE_DIR src/Android
    LIBS WNFibers
    SOURCES
      WNContextAndroid
  )
endif()