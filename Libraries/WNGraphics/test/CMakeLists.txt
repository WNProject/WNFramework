wn_create_tests_from_list(TEST_PREFIX WNGraphics
  SOURCE_DIR src/Tests
  SOURCES
    WNDownloadHeap
    WNQueryAdapters
    WNResourceCopy
    WNUploadHeap
    WNFence
  COMMON_SOURCES
    inc/WNTestFixture.h
  LIBS WNGraphics
)
