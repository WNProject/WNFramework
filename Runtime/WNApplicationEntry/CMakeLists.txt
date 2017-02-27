enable_overlay_directory()

add_wn_library(WNApplicationEntry
  SOURCES
    inc/WNApplicationEntry.h
    src/WNApplicationEntry.cpp
  LIBS
    WNMultiTasking
    WNApplicationData
)

wn_add_subdirectory(test)

post_enable_overlay_directory()