set_overlay_var(WN_DEFAULT_WINDOW_TYPE "android_window")
set_overlay_var(WN_DEFAULT_WINDOW_INCLUDE_PATH "WNWindow/inc/WNAndroidWindow.h")

if(ANDROID_NDK)
  file(TO_CMAKE_PATH ${ANDROID_NDK} NDK_PATH)
endif()

overlay_library_sources(
  WNWindow
  SOURCES
    inc/WNAndroidWindow.h
    src/WNAndroidWindow.cpp
  LIBS
    android
  INCLUDES
    "${NDK_PATH}/sources/android/native_app_glue"
)