
add_wn_library(WNFileSystemDeprecated SOURCES
  inc/WNFile.h
  inc/WNFileBuffer.h
  inc/WNPath.h
  inc/WNReadFileBuffer.h
  inc/WNWriteFileBuffer.h
  src/WNReadFileBuffer.cpp
  src/WNWriteFileBuffer.cpp
  src/WNFile.cpp
  src/WNFileBuffer.cpp)

target_include_directories(WNFileSystemDeprecated
  PUBLIC
  ${CMAKE_SOURCE_DIR}/Libraries)

if(ANDROID)
  set_property(TARGET WNFileSystemDeprecated PROPERTY WN_ANDROID_PERMISSIONS
    WRITE_EXTERNAL_STORAGE
    READ_EXTERNAL_STORAGE)
endif()
