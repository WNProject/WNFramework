include_directories(${WNFramework_SOURCE_DIR}/Libraries)
include_directories(${WNFramework_BINARY_DIR}/Libraries)
foreach(overlay ${WN_ENABLED_OVERLAYS})
  include_directories(${overlay}/Libraries)
endforeach()

enable_overlay_file()