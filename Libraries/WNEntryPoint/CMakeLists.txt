set(WNEntryPointSources)

set(WN_ANDROID_SOURCES src/Android/WNEntry.cpp)
set(WN_WINDOWS_SOURCES src/Windows/WNEntry.cpp)
set(WN_POSIX_SOURCES src/Linux/WNEntry.cpp)

if (WN_SYSTEM STREQUAL "Android")
  list(APPEND WNEntryPointSources ${WN_ANDROID_SOURCES})
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
  list(APPEND WNEntryPointSources ${WN_POSIX_SOURCES})
elseif (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WNEntryPointSources ${WN_WINDOWS_SOURCES}
  ${WN_POSIX_SOURCES} ${WN_ANDROID_SOURCES})
endif()

add_wn_library(WNEntryPoint SOURCES ${WNEntryPointSources}
  PRE_LINK_FLAGS ${WNEntryPreLinkFlags}
  POST_LINK_FLAGS ${WNEntryPostLinkFlags})

wn_target_link_libraries(WNEntryPoint ${WNAdditionalEntryLibs}
  WNUtils)

target_include_directories(WNEntryPoint
  PUBLIC
  ${CMAKE_SOURCE_DIR}/Libraries)

if (WN_SYSTEM STREQUAL "Windows")
  source_group("src\\Windows" FILES ${WN_WINDOWS_SOURCES})
  source_group("src\\Posix" FILES ${WN_POSIX_SOURCES})
  source_group("src\\Android" FILES ${WN_ANDROID_SOURCES})
  set_source_files_properties(${WN_POSIX_SOURCES} ${WN_ANDROID_SOURCES} 
    PROPERTIES HEADER_FILE_ONLY true)
endif()
