enable_overlay_directory()

add_wn_library(
  profiling
  SOURCES
    inc/allocator.h
    inc/profiling.h
    src/allocator.cpp
    src/profiling.cpp
  PUBLIC_COMPILE_DEFINES
    WN_TRACY_CALLBACK_DEPTH=${WN_TRACY_CALLBACK_DEPTH}
  INCLUDES
    ${WNFramework_SOURCE_DIR}/externals
  LIBS
    tracy_client
)

post_enable_overlay_directory()
