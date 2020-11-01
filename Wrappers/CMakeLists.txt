foreach(overlay ${WN_ENABLED_OVERLAYS})
  include_directories(${overlay}/Wrappers)
endforeach()

wn_add_subdirectory(stb)
