
set(SRC_ANDROID
    src/Android/WNTestHarness.cpp
)
set(SRC_POSIX
    src/Posix/WNTestHarness.cpp
)
set(SRC_WINDOWS
    src/Windows/WNTestHarness.cpp
)

set(WNTestingSources
   inc/WNTestHarness.h
   src/WNTestMain.cpp
)

if (WN_SYSTEM STREQUAL "Android")
  list(APPEND WNTestingSources
    ${SRC_ANDROID})
elseif (WN_POSIX)
  list(APPEND WNTestingSources
    ${SRC_POSIX})
elseif (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WNTestingSources
  ${SRC_WINDOWS} ${SRC_POSIX} ${SRC_ANDROID})
endif()
add_wn_library(WNTesting SOURCES
  ${WNTestingSources}
)

wn_target_link_libraries(WNTesting
  WNEntryPoint WNUtils gmock
)

if (WN_SYSTEM STREQUAL "Windows")
  source_group("src\\Windows" FILES ${SRC_WINDOWS})
  source_group("src\\Posix" FILES ${SRC_POSIX})
  source_group("src\\Android" FILES ${SRC_ANDROID})
  set_source_files_properties(${SRC_POSIX} ${SRC_ANDROID}
    PROPERTIES HEADER_FILE_ONLY true)
endif()

target_include_directories(WNTesting
  PUBLIC
    ${CMAKE_SOURCE_DIR}/Libraries
    ${gmock_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}/include
)
