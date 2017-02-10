set_overlay_var(WN_DEFAULT_WINDOW_TYPE "windows_window")
set_overlay_var(WN_DEFAULT_WINDOW_INCLUDE_PATH "WNWindow/inc/WNWindowsWindow.h")

overlay_library_sources(
  WNWindow
  SOURCES
    inc/WNWindowsWindow.h
    src/WNWindowsWindow.cpp
)