set(ENABLED OFF)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(ENABLED ON)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows64")
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} Windows)