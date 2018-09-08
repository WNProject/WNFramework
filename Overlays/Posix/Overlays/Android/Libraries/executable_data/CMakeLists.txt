if(ANDROID_NDK)
  file(TO_CMAKE_PATH ${ANDROID_NDK} NDK_PATH)
endif()

overlay_library_sources(executable_data
  SOURCES
    inc/host_specific_data.h
  INCLUDES
    "${NDK_PATH}/sources/android/native_app_glue"
)
