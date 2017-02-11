enable_overlay_directory()

add_wn_library(WNApplication
  SOURCES
    inc/WNApplicationData.h
    src/WNApplicationEntry.cpp
  LIBS
    WNMultiTasking
)

target_include_directories(WNApplication
  PUBLIC  
  ${WNRuntime_SOURCE_DIR})

add_subdirectory(test)