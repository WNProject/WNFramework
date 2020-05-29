set(ENABLED OFF)
if (WN_SYSTEM_NAME STREQUAL "Linux")
  set(ENABLED ON)
endif()
if (ANDROID)
  set(ENABLED ON)
endif()

if (WN_ANDROID_WRAPPER)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} NonMac)
