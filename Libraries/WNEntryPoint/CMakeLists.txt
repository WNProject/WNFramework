set(WNEntryPointSources)

if (WN_SYSTEM STREQUAL "Android")
  list(APPEND WNEntryPointSources src/Android/WNEntry.cpp)
  # We should be building android_native_app_glue here.
  add_wn_library(WNAndroidAppGlue SOURCES
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
  wn_target_link_libraries(WNAndroidAppGlue log android)
  target_include_directories(WNAndroidAppGlue PUBLIC
    ${ANDROID_NDK}/sources/android/native_app_glue)
  set(WNAdditionalEntryLibs WNAndroidAppGlue android log)
  set(WNEntryPreLinkFlags -Wl,--whole-archive)
  set(WNEntryPostLinkFlags -Wl,--no-whole-archive)
elseif (WN_POSIX)
  list(APPEND WNEntryPointSources src/Linux/WNEntry.cpp)
elseif (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WNEntryPointSources src/Windows/WNEntry.cpp)
endif()

add_wn_library(WNEntryPoint SOURCES ${WNEntryPointSources}
  PRE_LINK_FLAGS ${WNEntryPreLinkFlags}
  POST_LINK_FLAGS ${WNEntryPostLinkFlags})

wn_target_link_libraries(WNEntryPoint ${WNAdditionalEntryLibs}
  WNUtils)

target_include_directories(WNEntryPoint
  PUBLIC
  ${CMAKE_SOURCE_DIR}/Libraries)


