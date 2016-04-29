set_source_groups()
if (${name}_OVERLAY_DISABLED_SOURCES)
  list(APPEND ${name}_OVERLAY_SOURCES ${${name}_OVERLAY_DISABLED_SOURCES})
endif()