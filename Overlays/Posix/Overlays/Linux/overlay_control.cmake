set(ENABLED OFF)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} Linux)