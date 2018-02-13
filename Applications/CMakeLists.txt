include_directories(${WNFramework_SOURCE_DIR}/Libraries)
include_directories(${WNFramework_BINARY_DIR}/Libraries)
include_directories(${WNFramework_SOURCE_DIR}/Runtime)
include_directories(${WNFramework_BINARY_DIR}/Runtime)

set(REVERSE_OVERLAYS ${WN_ENABLED_OVERLAYS})

list(REVERSE REVERSE_OVERLAYS)

foreach(overlay ${REVERSE_OVERLAYS})
  include_directories(${overlay}/Libraries)
  include_directories(${overlay}/Wrappers)
  include_directories(${overlay}/Runtime)
endforeach()

wn_add_subdirectory(script_test_runner)
