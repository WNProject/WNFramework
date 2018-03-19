set_overlay_var(WN_DEFAULT_WINDOW_TYPE "xcb_window")
set_overlay_var(WN_DEFAULT_WINDOW_INCLUDE_PATH "WNWindow/inc/WNXCBWindow.h")

overlay_library_sources(
  WNWindow
  SOURCES
    inc/WNXCBWindow.h
    src/WNXCBWindow.cpp
  LIBS
    xcb
    xcb-keysyms
)