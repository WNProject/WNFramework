enable_overlay_directory()

wn_create_executable_tests_from_list(TEST_PREFIX WNGraphics
  SOURCE_DIR src
  SYSTEMS_TEST
  SOURCES
    WNArena
    WNDownloadHeap
    WNImage
    WNQueryAdapters
    WNResourceCopy
    WNUploadHeap
    WNFence
    WNPipeline
  COMMON_SOURCES
    inc/WNTestFixture.h
    shaders/pipeline.ps.glsl.h
    shaders/pipeline.ps.hlsl.h
    shaders/pipeline.vs.glsl.h
    shaders/pipeline.vs.hlsl.h
  LIBS
    WNGraphics
)

wn_create_application_tests_from_list(
  TEST_PREFIX WNGraphics
  SOURCE_DIR src
  SOURCES
    WNSwapchain
  LIBS
    WNGraphics
    WNWindow
)

post_enable_overlay_directory()