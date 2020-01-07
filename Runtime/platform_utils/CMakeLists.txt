enable_overlay_directory()

add_wn_library(platform_utils
  SOURCES
    inc/subprocess.h
  LIBS
    WNMultiTasking
)

wn_add_subdirectory(test)

post_enable_overlay_directory()
