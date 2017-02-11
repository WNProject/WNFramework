include_directories(${WNFramework_SOURCE_DIR}/Runtime)
include_directories(${WNFramework_BINARY_DIR}/Runtime)
foreach(overlay ${WN_ENABLED_OVERLAYS})
  include_directories(${overlay}/Runtime)
endforeach()

add_subdirectory(WNApplication)