set(ENABLED OFF)

if (ANDROID)
  set(ENABLED ON)

  if (NOT ${ANDROID_NATIVE_API_LEVEL} GREATER 30)
    message(FATAL_ERROR
      "Android api level must be >= 31 \"-DANDROID_NATIVE_API_LEVEL=31\"")
  endif()
endif()

if (WN_ANDROID_WRAPPER)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} Android)
