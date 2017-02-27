enable_overlay_directory()

add_wn_library(WNExecutableTest
  SOURCES
    inc/WNTestHarness.h
    src/WNTestMain.cpp
  INCLUDES
    ${gmock_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}/include
  LIBS
    WNExecutable
    WNTestUtilities
)

post_enable_overlay_directory()