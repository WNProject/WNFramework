foreach(WN_ENABLED_OVERLAY ${WN_ENABLED_OVERLAYS})
  include_directories(${WN_ENABLED_OVERLAY}/wrappers)
endforeach()

wn_add_subdirectory(stb)
