wn_create_tests_from_list(TEST_PREFIX WNGraphics
  SOURCE_DIR src/Tests
  SOURCES
    WNDownloadHeap
    WNQueryPhysicalDevice
    WNUploadHeap
  COMMON_SOURCES
    inc/WNTestFixture.h
  LIBS WNGraphics
)
