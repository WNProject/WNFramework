set(INC_INTERNAL
  inc/Internal/WNTime.inl
)
set(INC_INTERNAL_ANDROID
  inc/Internal/Android/WNSurfaceAndroid.h
  inc/Internal/Android/WNSurfaceManagerAndroid.h
)
set(INC_INTERNAL_LINUX
  inc/Internal/Linux/WNTime.inl
)
set(INC_INTERNAL_WINDOWS
  inc/Internal/Windows/WNSurfaceManagerWindows.h
  inc/Internal/Windows/WNSurfaceWindows.h
  inc/Internal/Windows/WNTime.inl
)
set(INC_INTERNAL_XWINDOWS
  inc/Internal/XWindows/WNSurfaceManagerXWindows.h
  inc/Internal/XWindows/WNSurfaceXWindows.h
)
source_group("inc\\Internal" FILES ${INC_INTERNAL})
source_group("inc\\Internal\\Linux" FILES ${INC_INTERNAL_LINUX})
source_group("inc\\Internal\\Android" FILES ${INC_INTERNAL_ANDROID})
source_group("inc\\Internal\\Windows" FILES ${INC_INTERNAL_WINDOWS})
source_group("inc\\Internal\\XWindows" FILES ${INC_INTERNAL_XWINDOWS})


set(WNPlatformSources
  inc/WNPlatform.h
  inc/WNPlatformFactory.h
  inc/WNSurface.h
  inc/WNSurfaceManager.h
  inc/WNTime.h
  src/WNPlatformFactory.cpp
  ${INC_INTERNAL}
  ${INC_INTERNAL_LINUX}
  ${INC_INTERNAL_ANDROID}
  ${INC_INTERNAL_WINDOWS}
  ${INC_INTERNAL_XWINDOWS})

set(WNPlatformLinkLibraries
  "")

if (WN_SYSTEM STREQUAL "Android")
  list(APPEND WNPlatformSources
    src/Android/WNSurfaceAndroid.cpp
    src/Android/WNSurfaceManagerAndroid.cpp)
  set(WNAdditionalPlatformIncludes
    ${ANDROID_NDK}/sources/android/native_app_glue)
  list(APPEND WNPlatformLinkLibraries
    EGL WNUtils)
elseif (WN_SYSTEM STREQUAL "Linux")
  list(APPEND WNPlatformSources
    src/XWindows/WNSurfaceManagerXWindows.cpp
    src/XWindows/WNSurfaceXWindows.cpp)
  list(APPEND WNPlatformLinkLibraries
    X11 GL)
elseif (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WNPlatformSources
    src/Windows/WNSurfaceManagerWindows.cpp
    src/Windows/WNSurfaceWindows.cpp)
endif()

add_wn_library(WNPlatform SOURCES
  ${WNPlatformSources}
  )

wn_target_link_libraries(WNPlatform PUBLIC ${WNPlatformLinkLibraries})
target_include_directories(WNPlatform PUBLIC
  ${CMAKE_SOURCE_DIR}/Libraries
  ${WNAdditionalPlatformIncludes})

add_subdirectory(test)
