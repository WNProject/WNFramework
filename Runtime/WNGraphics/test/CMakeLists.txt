enable_overlay_directory()

set(CMAKE_TEST_IMAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/golden_images)
configure_file(inc/WNPixelTestConfig.h.in inc/WNPixelTestConfig.h)

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

macro(add_pixel_test name)
  add_wn_executable(${name}
    SOURCES
      ${PIXEL_TEST_SOURCES}
    LIBS
      executable_test
      WNGraphics
      WNFileSystem
      stb
  INCLUDES
    ${WNFramework_SOURCE_DIR}/Externals/stb_image
  )

  wn_create_call_test(
    ${name}
      WORKING_DIRECTORY
        ${WNFramework_BINARY_DIR}
      APPLICATION
        ${name}
      COMMAND_PRE_DIR
        -image_path
      COMMAND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/golden_images
  )
endmacro()

set(PIXEL_TEST_SOURCES
  src/WNTriangleTests.cpp
  inc/WNPixelTestFixture.h
  inc/WNPixelTestConfig.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/inc/WNPixelTestConfig.h
  shaders/pipeline.ps.glsl.h
  shaders/pipeline.ps.hlsl.h
  shaders/pipeline.vs.glsl.h
  shaders/pipeline.vs.hlsl.h
)

add_pixel_test(graphics_pixel_triangle_test)

set(PIXEL_TEST_SOURCES
    src/WNTexturedTriangleTests.cpp
    inc/WNPixelTestFixture.h
    inc/WNPixelTestConfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/WNPixelTestConfig.h
    shaders/textured_pipeline.ps.glsl.h
    shaders/textured_pipeline.ps.hlsl.h
    shaders/textured_pipeline.vs.glsl.h
    shaders/textured_pipeline.vs.hlsl.h
)

add_pixel_test(graphics_pixel_textured_triangle_test)

wn_filesystem_files(push_constants_test
  OUTPUT_DIR inc
  SOURCES
    push_constants/built_shaders/pipeline.ps.dxbc
    push_constants/built_shaders/pipeline.vs.dxbc
    push_constants/built_shaders/pipeline.ps.spv
    push_constants/built_shaders/pipeline.vs.spv
)

set(PIXEL_TEST_SOURCES
    src/WNPushConstantsTests.cpp
    inc/WNPixelTestFixture.h
    inc/WNPixelTestConfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/WNPixelTestConfig.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/push_constants_test.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/push_constants_test.cpp
)

add_pixel_test(graphics_pixel_push_constants_test)

wn_filesystem_files(astc_test
    OUTPUT_DIR inc
    SOURCES
        assets/gradient_astc.img
)

set(PIXEL_TEST_SOURCES
    src/WNASTCTest.cpp
    inc/WNPixelTestFixture.h
    inc/WNPixelTestConfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/WNPixelTestConfig.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/astc_test.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/astc_test.cpp
    shaders/textured_pipeline.ps.glsl.h
    shaders/textured_pipeline.ps.hlsl.h
    shaders/textured_pipeline.vs.glsl.h
    shaders/textured_pipeline.vs.hlsl.h
)
add_pixel_test(graphics_pixel_test_astc_texture)

wn_filesystem_files(bc_test
    OUTPUT_DIR inc
    SOURCES
        assets/gradient_bc.img
)

set(PIXEL_TEST_SOURCES
    src/WNBCTest.cpp
    inc/WNPixelTestFixture.h
    inc/WNPixelTestConfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/WNPixelTestConfig.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/bc_test.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/bc_test.cpp
    shaders/textured_pipeline.ps.glsl.h
    shaders/textured_pipeline.ps.hlsl.h
    shaders/textured_pipeline.vs.glsl.h
    shaders/textured_pipeline.vs.hlsl.h
)
add_pixel_test(graphics_pixel_test_bc_texture)

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

post_enable_overlay_directory()