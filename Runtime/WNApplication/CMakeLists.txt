enable_overlay_directory()

add_wn_library(WNApplication
  SOURCES
    inc/WNApplicationData.h
    src/WNApplicationEntry.cpp
  LIBS
    WNMultiTasking
)

add_subdirectory(test)