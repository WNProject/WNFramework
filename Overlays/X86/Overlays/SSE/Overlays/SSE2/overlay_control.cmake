set(ENABLED OFF)
if (WN_ENABLE_SSE2)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} SSE2)
