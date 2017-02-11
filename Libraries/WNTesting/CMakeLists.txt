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

add_wn_library(WNTesting
  SOURCES
    inc/WNTestHarness.h
    src/WNTestMain.cpp
  INCLUDES
    ${gmock_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}/include
  LIBS
    WNEntryPoint
    WNUtilities
    gmock
)
