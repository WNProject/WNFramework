enable_overlay_directory()

wn_create_commandline_application_tests_from_list(
  TEST_PREFIX networking_rt
  SOURCE_DIR src
  SOURCES
    basic
  LIBS
    networking_rt
)

post_enable_overlay_directory()