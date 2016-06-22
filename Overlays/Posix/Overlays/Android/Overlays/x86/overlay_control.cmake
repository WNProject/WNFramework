set(ENABLED OFF)

if (WN_ARCHITECTURE STREQUAL "x86")
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} x86)
