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

source_group("inc\\Internal\\Linux" FILES ${INC_INTERNAL_LINUX})
source_group("inc\\Internal\\Windows" FILES ${INC_INTERNAL_WINDOWS})

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
    src/Linux/WNListenConnectionLinux.cpp
    src/Linux/WNOutConnectionLinux.cpp
    src/Linux/WNConnectionLinux.cpp
    src/Linux/WNNetworkManagerLinux.cpp
    src/Linux/WNListenConnectionLinux.cpp
  )
elseif (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WNNetworkingSources
    src/Windows/WNOutConnectionWindows.cpp
    src/Windows/WNListenConnectionWindows.cpp
    src/Windows/WNConnectionWindows.cpp
    src/Windows/WNInConnectionWindows.cpp
    src/Windows/WNNetworkManagerWindows.cpp
    )
endif()

add_wn_library(WNNetworking SOURCES
  ${WNNetworkingSources})


target_include_directories(WNNetworking PUBLIC
  ${CMAKE_SOURCE_DIR}/Libraries)
