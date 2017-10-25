enable_overlay_directory()

add_wn_executable(
  script_test_runner
  SOURCES
    src/WNScriptTestRunner.cpp
  LIBS
    WNScripting
    WNRe2
    effcee
    WNExecutable
  INCLUDES
    ${WNFramework_SOURCE_DIR}/Externals/effcee
)

post_enable_overlay_directory()