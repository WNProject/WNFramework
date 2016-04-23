enable_overlay_directory()

add_wn_library(WNPlatform
  SOURCES
    inc/WNPlatformFactory.h
    inc/WNSurface.h
    inc/WNSurfaceManager.h
    src/WNPlatformFactory.cpp
)

add_subdirectory(test)
