set(ENABLED OFF)

if (WN_SYSTEM_NAME STREQUAL "Windows")
  set(ENABLED ON)
endif()

if (WN_SYSTEM_NAME STREQUAL "Windows64")
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} Windows)