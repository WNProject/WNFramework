enable_overlay_directory()

add_wn_application(
  engine
  SOURCES
    inc/script_export.h
    src/main.cpp
    src/script_export.cpp
  LIBS
    WNScripting
    engine_base
    profiling
    support
    renderer
    window
    ui
)

post_enable_overlay_directory()