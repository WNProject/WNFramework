
overlay_library_sources(WNMultiTasking
  SOURCES
    src/Android/WNContext.h
    src/Android/WNMakeContext.cpp
    src/Android/WNSwapContext.cpp)

wn_add_subdirectory(test)
