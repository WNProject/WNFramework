enable_overlay_directory()

add_wn_executable(
  script_test_runner
  SOURCES
    src/WNScriptTestRunner.cpp
  INCLUDES
    ${WNFramework_SOURCE_DIR}/externals/effcee
    ${WNFramework_SOURCE_DIR}/externals/re2
  LIBS
    WNScripting
    executable_entry
    effcee
    re2
)

post_enable_overlay_directory()
