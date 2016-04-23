enable_overlay_directory()

add_subdirectory(${WNFramework_SOURCE_DIR}/Externals/googletest gtest)

add_wn_library(WNTesting
  SOURCES
    inc/WNTestHarness.h
    src/WNTestMain.cpp
  INCLUDES
    ${gmock_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}/include
  LIBS
    WNEntryPoint WNUtilities gmock
)
