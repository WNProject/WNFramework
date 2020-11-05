set(WN_TEST_UTILITIES_GOOGLETEST_COMPILE_OPTIONS)

enable_overlay_directory()

add_wn_externals(
  googletest
  DIRECTORY
    ${WNFramework_SOURCE_DIR}/externals/googletest
  COMPILE_OPTIONS
    ${WN_TEST_UTILITIES_GOOGLETEST_COMPILE_OPTIONS}
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

post_enable_overlay_directory()
