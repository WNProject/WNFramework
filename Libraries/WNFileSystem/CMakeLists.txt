enable_overlay_directory()

if (NOT WN_FILE_SYSTEM_MAPPING_H OR NOT WN_FILE_SYSTEM_SYSTEM_MAPPING_TYPE)
  message(ERROR "You must have a system specific file system mapping set")
endif()

message(STATUS "File System Mapping Type: ${WN_FILE_SYSTEM_SYSTEM_MAPPING_TYPE}")

configure_file(src/WNSystemMapping.h.in
  src/WNSystemMapping.h)

add_wn_library(WNFileSystem
  SOURCES
    inc/WNFactory.h
    inc/WNFile.h
    inc/WNMapping.h
    inc/WNResult.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/WNSystemMapping.h
    src/WNFactory.cpp
    src/WNMapping.cpp
    src/WNMemoryBackedMapping.cpp
    src/WNMemoryBackedMapping.h
    src/WNSystemMapping.h.in
    src/WNUtilities.h
  LIBS
    WNUtilities
)

wn_add_subdirectory(test)

post_enable_overlay_directory()