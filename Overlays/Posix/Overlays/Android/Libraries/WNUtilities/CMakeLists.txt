add_wn_library(AndroidNativeAppGlue
  SOURCES
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
  INCLUDES
    ${ANDROID_NDK}/sources/android/native_app_glue)


overlay_library_sources(WNUtilities
  SOURCES
    inc/WNAndroidEventPump.h
    inc/WNAppData.h
    inc/WNLoggingData.h
    src/WNAndroidEventPump.cpp
    src/WNAppData.cpp
    src/WNCrashHandler.cpp
    src/WNLoggingData.cpp
  INCLUDES
    ${ANDROID_NDK}/sources/android/native_app_glue
  LIBS
    AndroidNativeAppGlue)
