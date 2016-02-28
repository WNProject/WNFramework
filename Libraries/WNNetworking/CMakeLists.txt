set(SRC_LINUX
  src/Linux/WNListenConnectionLinux.cpp
  src/Linux/WNOutConnectionLinux.cpp
  src/Linux/WNConnectionLinux.cpp
  src/Linux/WNNetworkManagerLinux.cpp
  src/Linux/WNListenConnectionLinux.cpp
)

set(SRC_WINDOWS
    src/Windows/WNOutConnectionWindows.cpp
    src/Windows/WNListenConnectionWindows.cpp
    src/Windows/WNConnectionWindows.cpp
    src/Windows/WNInConnectionWindows.cpp
    src/Windows/WNNetworkManagerWindows.cpp
)

set(INC_INTERNAL_LINUX
  inc/Internal/Linux/WNConnectionLinux.h
  inc/Internal/Linux/WNInConnectionLinux.h
  inc/Internal/Linux/WNListenConnectionLinux.h
  inc/Internal/Linux/WNNetworkManagerLinux.h
  inc/Internal/Linux/WNOutConnectionLinux.h
)

set(INC_INTERNAL_WINDOWS
  inc/Internal/Windows/WNBufferResource.inl
  inc/Internal/Windows/WNConnectionWindows.h
  inc/Internal/Windows/WNInConnectionWindows.h
  inc/Internal/Windows/WNListenConnectionWindows.h
  inc/Internal/Windows/WNNetworkManagerWindows.h
  inc/Internal/Windows/WNOutConnectionWindows.h
)

set(WNNetworkingSources
  inc/WNBufferResource.h
  inc/WNConnection.h
  inc/WNConnectionGroup.h
  inc/WNNetworkFactory.h
  inc/WNNetworkManager.h
  inc/WNNetworkReadBuffer.h
  inc/WNNetworkWriteBuffer.h
  src/WNNetworkReadBuffer.cpp
  src/WNBufferResource.cpp
  src/WNConnection.cpp
  src/WNNetworkWriteBuffer.cpp
  src/WNNetworkFactory.cpp
  src/WNConnectionGroup.cpp
  src/WNNetworkManager.cpp
  ${INC_INTERNAL_LINUX}
  ${INC_INTERNAL_WINDOWS}
)

if (WN_POSIX)
  list(APPEND WNNetworkingSources
    ${SRC_LINUX}
  )
elseif (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WNNetworkingSources
    ${SRC_WINDOWS} ${SRC_LINUX}   
  )
endif()

add_wn_library(WNNetworking SOURCES
  ${WNNetworkingSources})

if(ANDROID)
  set_property(TARGET WNNetworking PROPERTY WN_ANDROID_PERMISSIONS
    INTERNET)
endif()

if (WN_SYSTEM STREQUAL "Windows")
	set_source_files_properties(${SRC_LINUX} 
		PROPERTIES HEADER_FILE_ONLY true)
endif()