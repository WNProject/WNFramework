set(INC_INTERNAL
  inc/Internal/WNTime.inl
)

set(INC_INTERNAL_ANDROID
  inc/Internal/Android/WNSurfaceAndroid.h
  inc/Internal/Android/WNSurfaceManagerAndroid.h
)

set(SRC_ANDROID
  src/Android/WNSurfaceAndroid.cpp
  src/Android/WNSurfaceManagerAndroid.cpp
)

set(INC_INTERNAL_LINUX
  inc/Internal/Linux/WNTime.inl
)

set(SRC_XWINDOWS
  src/XWindows/WNSurfaceManagerXWindows.cpp
  src/XWindows/WNSurfaceXWindows.cpp
)

set(INC_INTERNAL_WINDOWS
  inc/Internal/Windows/WNSurfaceManagerWindows.h
  inc/Internal/Windows/WNSurfaceWindows.h
  inc/Internal/Windows/WNTime.inl
)

set(SRC_WINDOWS
  src/Windows/WNSurfaceManagerWindows.cpp
  src/Windows/WNSurfaceWindows.cpp
)

set(INC_INTERNAL_XWINDOWS
  inc/Internal/XWindows/WNSurfaceManagerXWindows.h
  inc/Internal/XWindows/WNSurfaceXWindows.h
)

set(SRC_XWINDOWS
  src/XWindows/WNSurfaceManagerXWindows.cpp
  src/XWindows/WNSurfaceXWindows.cpp
)

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
  WNMultiTasking)

if (WN_SYSTEM STREQUAL "Android")
  list(APPEND WNPlatformSources
    ${SRC_ANDROID}
  )
  set(WNAdditionalPlatformIncludes
    ${ANDROID_NDK}/sources/android/native_app_glue)
  list(APPEND WNPlatformLinkLibraries
    EGL WNUtilities)
elseif (WN_SYSTEM STREQUAL "Linux")
  list(APPEND WNPlatformSources
    ${SRC_XWINDOWS})
  list(APPEND WNPlatformLinkLibraries
    X11 GL)
elseif (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WNPlatformSources
    ${SRC_WINDOWS} ${SRC_XWINDOWS} ${SRC_ANDROID})
endif()

add_wn_library(WNPlatform SOURCES
  ${WNPlatformSources}
  )

if (WN_SYSTEM STREQUAL "Windows")
	set_source_files_properties(${SRC_XWINDOWS} ${SRC_ANDROID} 
		PROPERTIES HEADER_FILE_ONLY true)
endif()

wn_target_link_libraries(WNPlatform PUBLIC ${WNPlatformLinkLibraries})
target_include_directories(WNPlatform PUBLIC
  ${WNAdditionalPlatformIncludes})

add_subdirectory(test)
