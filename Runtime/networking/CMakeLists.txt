enable_overlay_directory()

add_wn_library(networking_rt
  SOURCES
    inc/manager.h
    inc/connection.h
    inc/routed_connection.h
    src/dummy.cpp
  LIBS
    WNNetworking
)

wn_add_subdirectory(test)

post_enable_overlay_directory()
