set(WN_DEFAULT_WINDOW_TYPE "null_window")
set(WN_DEFAULT_WINDOW_INCLUDE_PATH "WNWindow/inc/WNNullWindow.h")

enable_overlay_directory()

add_wn_library(WNWindow
  SOURCES
    inc/WNWindowErrors.h
    src/WNWindowFactory.cpp
    inc/WNWindowFactory.h
    inc/Internal/WNConfig.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNConfig.h
    inc/Internal/WNWindowInclude.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNWindowInclude.h
    inc/Internal/WNConfig.h.in
    src/WNWindow.cpp
    inc/WNWindow.h
    inc/WNWindowEnums.h
    inc/WNNullWindow.h
  LIBS
    WNMultiTasking
    WNApplicationData
)

option(WN_ALLOW_DEFAULT_NULL_WINDOW
  "If no window type could be determined allow the use of null_window as the default"
  ${WN_ALLOW_DEFAULT_NULL_WINDOW})
# TODO(awoloszyn): Disable null window by default once we have a window
# for all supported platform
#if (NOT ${WN_ALLOW_DEFAULT_NULL_WINDOW})
#  if (WN_DEFAULT_WINDOW_TYPE STREQUAL "null_window")
#    message(FATAL_ERROR "Could not determine the default window type")
#  endif()
#endif()

message(STATUS "Default window type: ${WN_DEFAULT_WINDOW_TYPE}")

configure_file(inc/Internal/WNConfig.h.in
  inc/Internal/WNConfig.h)
configure_file(inc/Internal/WNWindowInclude.h.in
  inc/Internal/WNWindowInclude.h)

add_subdirectory(test)