enable_overlay_directory()

add_wn_externals(googletest
  DIRECTORY
    ${WNFramework_SOURCE_DIR}/Externals/googletest
  TARGETS
    gtest
    gtest_main
    gmock
    gmock_main
)

add_wn_library(WNTestUtilities
  SOURCES
    inc/WNTestHarness.h
    src/WNTestDummy.cpp
  INCLUDES
    ${gmock_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}/include
  LIBS
    gmock
)
