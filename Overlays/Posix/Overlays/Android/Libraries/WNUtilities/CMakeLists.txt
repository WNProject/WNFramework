if(ANDROID_NDK)
  file(TO_CMAKE_PATH ${ANDROID_NDK} NDK_PATH)
endif()

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
    ${NDK_PATH}/sources/android/native_app_glue
)
