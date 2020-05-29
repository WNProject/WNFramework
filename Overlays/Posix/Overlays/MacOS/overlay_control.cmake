set(ENABLED OFF)
if (WN_SYSTEM_NAME STREQUAL "Darwin")
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} MacOS)
