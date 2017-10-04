enable_overlay_directory()

set(CMAKE_TEST_IMAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/golden_images)
configure_file(inc/WNPixelTestConfig.h.in
  inc/WNPixelTestConfig.h)

wn_create_executable_tests_from_list(TEST_PREFIX WNGraphics
  SOURCE_DIR src
  SYSTEMS_TEST
  SOURCES
    WNArena
    WNBuffer
    WNImage
    WNQueryAdapters
    WNResourceCopy
    WNFence
    WNPipeline
  COMMON_SOURCES
    inc/WNTestFixture.h
    shaders/pipeline.ps.glsl.h
    shaders/pipeline.ps.hlsl.h
    shaders/pipeline.vs.glsl.h
    shaders/pipeline.vs.hlsl.h
    shaders/textured_pipeline.ps.glsl.h
    shaders/textured_pipeline.ps.hlsl.h
    shaders/textured_pipeline.vs.glsl.h
    shaders/textured_pipeline.vs.hlsl.h
  LIBS
    WNGraphics
)

wn_create_executable_tests_from_list(TEST_PREFIX WNGraphicsPixel
  SOURCE_DIR src
  SYSTEMS_TEST
  ADDITIONAL_INCLUDES
    ${WNFramework_SOURCE_DIR}/Externals/stb_image
  SOURCES
    WNTriangle
    WNTexturedTriangle
  COMMON_SOURCES
    inc/WNPixelTestFixture.h
    inc/WNPixelTestConfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/WNPixelTestConfig.h
    shaders/pipeline.ps.glsl.h
    shaders/pipeline.ps.hlsl.h
    shaders/pipeline.vs.glsl.h
    shaders/pipeline.vs.hlsl.h
  LIBS
    WNGraphics
    stb
)

wn_create_application_tests_from_list(
  TEST_PREFIX WNGraphics
  SOURCE_DIR src
  SOURCES
    WNSwapchain
  COMMON_SOURCES
    shaders/pipeline.ps.glsl.h
    shaders/pipeline.ps.hlsl.h
    shaders/pipeline.vs.glsl.h
    shaders/pipeline.vs.hlsl.h
  LIBS
    WNGraphics
    WNWindow
)

wn_filesystem_files(push_constants_test
  OUTPUT_DIR inc
  SOURCES
    push_constants/built_shaders/pipeline.ps.dxbc
    push_constants/built_shaders/pipeline.vs.dxbc
    push_constants/built_shaders/pipeline.ps.spv
    push_constants/built_shaders/pipeline.vs.spv
)

wn_create_executable_tests_from_list(TEST_PREFIX WNGraphicsPixel
  SOURCE_DIR src
  SYSTEMS_TEST
  ADDITIONAL_INCLUDES
    ${WNFramework_SOURCE_DIR}/Externals/stb_image
  SOURCES
    WNPushConstants
  COMMON_SOURCES
    inc/WNPixelTestFixture.h
    inc/WNPixelTestConfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/WNPixelTestConfig.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/push_constants_test.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/push_constants_test.cpp
  LIBS
    WNGraphics
    WNFileSystem
    stb
)

post_enable_overlay_directory()