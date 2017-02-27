enable_overlay_directory()

add_wn_header_library(WNThreadSafeContainers
  SOURCES
    inc/WNObjectPool.h
)

wn_add_subdirectory(test)

post_enable_overlay_directory()