enable_overlay_directory()

add_wn_library(WNApplicationTest
  SOURCES
    inc/WNTestHarness.h
    src/WNTestMain.cpp
  INCLUDES
    ${gmock_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}/include
  LIBS
    WNApplicationData
    WNTestUtilities
)
