set(WN_PLATFORM)

if (WIN32 OR CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(WN_PLATFORM "Windows")
elseif (ANDROID OR CMAKE_SYSTEM_NAME MATCHES "Android")
  set(WN_PLATFORM "Android")
elseif (UNIX OR CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(WN_PLATFORM "Linux")
else()
  message(FATAL_ERROR "Failed to determine platform")
endif()

message(STATUS "Platform: ${WN_PLATFORM}")