set(ENABLED OFF)
if (ANDROID)
  set(ENABLED ON)
  if (NOT ${ANDROID_NATIVE_API_LEVEL} GREATER 18)
    message(FATAL_ERROR
      "Android api level must be >= 19 \"-DANDROID_NATIVE_API_LEVEL=19\"")
  endif()
endif()

register_overlay(${ENABLED} Android)
