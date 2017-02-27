set(ENABLED OFF)

if (ANDROID)
  set(ENABLED ON)
endif()
if (WN_ANDROID_WRAPPER)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} Android)
