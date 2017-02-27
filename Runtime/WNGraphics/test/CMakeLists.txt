enable_overlay_directory()

wn_create_executable_tests_from_list(TEST_PREFIX WNGraphics
  SOURCE_DIR src/Tests
  SYSTEMS_TEST
  SOURCES
    WNDownloadHeap
    WNImage
    WNQueryAdapters
    WNResourceCopy
    WNUploadHeap
    WNFence
  COMMON_SOURCES
    inc/WNTestFixture.h
  LIBS
    WNGraphics
)

wn_create_application_tests_from_list(
  TEST_PREFIX WNGraphics
  SOURCE_DIR src/Tests
  SOURCES
    WNSwapchain
  LIBS
    WNGraphics
    WNWindow
)

post_enable_overlay_directory()