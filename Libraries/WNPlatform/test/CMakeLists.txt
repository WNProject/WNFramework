set(WNPlatformTestsTool "")

if (WN_SYSTEM STREQUAL "Linux")
  set(WNPlatformTestsTool ${CMAKE_SOURCE_DIR}/Utilities/glx_test_wrapper.sh)
endif()

wn_create_tests_from_list(TEST_PREFIX WNPlatform
  SOURCE_DIR src/Tests
  SYSTEMS_TEST
  LIBS WNPlatform
  RUN_WRAPPER ${WNPlatformTestsTool}
  SOURCES
    WNSurface
  )
