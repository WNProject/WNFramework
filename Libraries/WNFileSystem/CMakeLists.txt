
add_wn_library(WNFileSystem
  inc/WNFile.h
  inc/WNFileBuffer.h
  inc/WNFileSystem.h
  inc/WNPath.h
  inc/WNReadFileBuffer.h
  inc/WNWriteFileBuffer.h
  src/WNReadFileBuffer.cpp
  src/WNWriteFileBuffer.cpp
  src/WNFile.cpp
  src/WNFileBuffer.cpp)

target_include_directories(WNFileSystem
  PUBLIC
  ${CMAKE_SOURCE_DIR}/Libraries)

