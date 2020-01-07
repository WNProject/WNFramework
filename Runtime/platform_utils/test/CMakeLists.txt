enable_overlay_directory()

wn_create_application_tests_from_list(
  TEST_PREFIX platform_utils
  SOURCE_DIR src
  SOURCES
    subprocess
  LIBS
    platform_utils
)

post_enable_overlay_directory()