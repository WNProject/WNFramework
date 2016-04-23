
overlay_library_sources(WNPlatform
  SOURCES
    inc/Internal/WNSurfaceAndroid.h
    inc/Internal/WNSurfaceManagerAndroid.h
    src/WNSurfaceAndroid.cpp
    src/WNSurfaceManagerAndroid.cpp
  LIBS
    EGL
    android
  INCLUDES
    ${ANDROID_NDK}/sources/android/native_app_glue
)
