set(ENABLED OFF)

if (WN_SYSTEM_NAME STREQUAL "Linux")
  set(ENABLED ON)
endif()

if (WN_SYSTEM_NAME STREQUAL "Android")
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} Posix)
