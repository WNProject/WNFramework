enable_overlay_directory()

include_directories(${WNFramework_BINARY_DIR}/Applications/demos)
include_directories(${WNFramework_SOURCE_DIR}/Externals/librocket/Include)

add_subdirectory(${WNFramework_SOURCE_DIR}/Externals/librocket librocket)

wn_filesystem_files(rocket_assets
  OUTPUT_DIR
    inc
  SOURCES
    assets/textures/gradient.png
    assets/main.rml
    assets/main.rcss
    assets/fonts/FiraCode-Regular.ttf
    assets/built_shaders/ui.vs.spv
    assets/built_shaders/ui.vs.dxbc
    assets/built_shaders/ui_tex.ps.spv
    assets/built_shaders/ui_tex.ps.dxbc
    assets/built_shaders/ui_no_tex.ps.spv
    assets/built_shaders/ui_no_tex.ps.dxbc
)


add_wn_application(rocket_test
  INCLUDES
     ${WNFramework_SOURCE_DIR}/Externals/stb
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

add_dependencies(rocket_test rocket_assets)

post_enable_overlay_directory()
