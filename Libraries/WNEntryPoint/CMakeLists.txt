set(WNEntryPointSources)

if (WN_SYSTEM STREQUAL "Android")
  list(APPEND WNEntryPointSources src/Android/WNEntry.cpp)
elseif (WN_POSIX)
  list(APPEND WNEntryPointSources src/Linux/WNEntry.cpp)
elseif (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WNEntryPointSources src/Windows/WNEntry.cpp)
endif()
add_wn_library(WNEntryPoint
  ${WNEntryPointSources})

target_link_libraries(WNEntryPoint
  WNUtils)

target_include_directories(WNEntryPoint
  PUBLIC
  ${CMAKE_SOURCE_DIR}/Libraries)


