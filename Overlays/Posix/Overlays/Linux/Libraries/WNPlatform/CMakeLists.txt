overlay_library_sources(WNPlatform
  SOURCES
    inc/Internal/WNSurfaceManagerXWindows.h
    inc/Internal/WNSurfaceXWindows.h
    src/WNSurfaceManagerXWindows.cpp
    src/WNSurfaceXWindows.cpp
  LIBS
    WNMultiTasking
    X11 GL
)
