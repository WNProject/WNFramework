enable_overlay_directory()

include_directories(${WNFramework_BINARY_DIR}/Applications/demos)
include_directories(${WNFramework_SOURCE_DIR}/Externals/librocket/Include)

add_subdirectory(${WNFramework_SOURCE_DIR}/Externals/librocket librocket)

wn_filesystem_files(rocket_assets
  OUTPUT_DIR
    inc
  SOURCES
    assets/built_shaders/ui.vs.spv
    assets/built_shaders/ui_no_tex.ps.spv
    assets/built_shaders/ui_tex.ps.spv
    assets/built_shaders/ui.vs.dxbc
    assets/built_shaders/ui_no_tex.ps.dxbc
    assets/built_shaders/ui_tex.ps.dxbc
    assets/fonts/lc_regular_12.fnt
    assets/fonts/lc_regular_20.fnt
    assets/fonts/lc_regular_12_0.png
    assets/fonts/lc_regular_20_0.png
    assets/fonts/lc_italic_12.fnt
    assets/fonts/lc_italic_20.fnt
    assets/fonts/lc_italic_12_0.png
    assets/fonts/lc_italic_20_0.png
    assets/textures/gradient.png
    assets/main.rml
    assets/main.rcss)


add_wn_application(rocket_test
  INCLUDES
     ${WNFramework_SOURCE_DIR}/Externals/stb_image
  SOURCES
    src/main.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/inc/rocket_assets.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/rocket_assets.cpp
  LIBS
    librocket
    stb
    WNWindow
    WNGraphics
    WNFileSystem
)

post_enable_overlay_directory()