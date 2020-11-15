enable_overlay_directory()

wn_create_application_tests_from_list(
  TEST_PREFIX WNWindow
  SOURCE_DIR src
  SOURCES
    WNWindow
  LIBS
    WNWindow
  LABELS
    REQUIRES_HARDWARE
)

post_enable_overlay_directory()
