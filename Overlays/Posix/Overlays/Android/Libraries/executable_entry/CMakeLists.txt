if(ANDROID_NDK)
  file(TO_CMAKE_PATH ${ANDROID_NDK} NDK_PATH)
endif()

overlay_library_sources(executable_entry
  SOURCES
    src/executable_entry.cpp
    ${NDK_PATH}/sources/android/native_app_glue/android_native_app_glue.c
  LIBS
    log
	android
  INCLUDES
    "${NDK_PATH}/sources/android/native_app_glue"
)
