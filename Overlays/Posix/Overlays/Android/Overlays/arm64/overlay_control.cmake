set(ENABLED OFF)

if (WN_ARCHITECTURE STREQUAL "ARM64")
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} arm64)
