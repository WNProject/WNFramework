enable_overlay_directory()

add_wn_application(
  engine
  SOURCES
    inc/script_export.h
    src/main.cpp
    src/script_export.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/inc/engine_scripts.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/engine_scripts.cpp
  LIBS
    WNScripting
    engine_base
    profiling
    support
    renderer
    window
    ui
)

wn_filesystem_files(engine_scripts
  OUTPUT_DIR
    inc
  REMOVE_PREFIX
    1
  SOURCES
    engine_scripts/renderer/pass_data.wns
    engine_scripts/renderer/render_data.wns
    engine_scripts/renderer/render_description.wns
    engine_scripts/renderer/rt_description.wns
    engine_scripts/renderer/texture_description.wns
    engine_scripts/ui/renderable_ui.wns
    engine_scripts/assets/built_shaders/ui_no_tex.ps.dxbc
    engine_scripts/assets/built_shaders/ui_no_tex.ps.spv
    engine_scripts/assets/built_shaders/ui_tex.ps.dxbc
    engine_scripts/assets/built_shaders/ui_tex.ps.spv
    engine_scripts/assets/built_shaders/ui.vs.dxbc
    engine_scripts/assets/built_shaders/ui.vs.spv
)

add_dependencies(engine engine_scripts)
post_enable_overlay_directory()