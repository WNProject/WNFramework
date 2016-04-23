set(ENABLED OFF)

if (WN_ARCHITECTURE STREQUAL "ARM")
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} arm)
