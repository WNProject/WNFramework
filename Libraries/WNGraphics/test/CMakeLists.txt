set(WNGraphicsTestsTool "")

if (WN_SYSTEM STREQUAL "Linux")
  set(WNGraphicsTestsTool ${CMAKE_SOURCE_DIR}/Utilities/glx_test_wrapper.sh)
endif()

wn_create_test(TEST_PREFIX WNGraphics
  TEST_NAME WNGraphics
  SOURCES src/Main.cpp
  LIBS WNGraphics
  RUN_WRAPPER ${WNGraphicsTestsTool}
  )
