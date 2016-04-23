set(ENABLED OFF)
option(WN_ENABLE_3DNOW "Enable 3DNow Support" ${WN_ENABLE_3DNOW})
if (WN_ENABLE_3DNOW)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} 3DNow)
