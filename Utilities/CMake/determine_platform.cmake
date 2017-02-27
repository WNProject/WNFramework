set(WN_PLATFORM)

if (WN_SYSTEM_NAME MATCHES "Windows")
  set(WN_PLATFORM "Windows")
elseif (WN_SYSTEM_NAME MATCHES "Android")
  set(WN_PLATFORM "Android")
elseif (WN_SYSTEM_NAME MATCHES "Linux")
  set(WN_PLATFORM "Linux")
else()
  message(FATAL_ERROR "Failed to determine platform")
endif()

message(STATUS "Platform: ${WN_PLATFORM}")