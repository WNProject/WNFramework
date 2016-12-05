enable_overlay_directory()

add_wn_library(WNNetworking
  SOURCES
    inc/WNConnection.h
    inc/WNNetworkingErrors.h
    inc/WNNetworkManager.h
    inc/WNReliableConnection.h
    inc/WNRoutedConnection.h
    inc/WNReceiveBuffer.h
    src/WNReceiveBuffer.cpp
  LIBS
    WNMultiTasking
)

add_subdirectory(test)
