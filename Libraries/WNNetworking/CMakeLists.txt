enable_overlay_directory()

add_wn_library(WNNetworking
  SOURCES
  inc/WNNetworkingErrors.h
  inc/WNNetworkManager.h
  inc/WNReliableConnection.h
  src/WNNetworkManager.cpp
)

add_subdirectory(test)
