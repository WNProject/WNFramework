set(ENABLED OFF)
if (WN_ENABLE_SSE4_1)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} SSE4_1)
