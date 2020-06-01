set_overlay_var(WN_DEFAULT_WINDOW_TYPE "macos_window")
set_overlay_var(WN_DEFAULT_WINDOW_INCLUDE_PATH "WNWindow/inc/WNMacOSWindow.h")

overlay_library_sources(
  WNWindow
  SOURCES
    inc/WNMacOSWindow.h
    inc/helpers.h
    src/WNMacOSWindow.cpp
    src/macos_window.m
  LIBS
    "-framework Cocoa"
    "-framework QuartzCore"
)