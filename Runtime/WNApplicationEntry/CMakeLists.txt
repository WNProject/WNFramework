enable_overlay_directory()

add_wn_library(WNApplicationEntry
  SOURCES
    inc/WNApplicationEntry.h
    src/WNApplicationEntry.cpp
  LIBS
    WNMultiTasking
    WNApplicationData
)

add_subdirectory(test)