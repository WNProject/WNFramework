enable_overlay_directory()

add_wn_library(WNFileSystem
  SOURCES
    inc/WNFactory.h
    inc/WNFile.h
    inc/WNMapping.h
    inc/WNResult.h
    src/WNFactory.cpp
    src/WNMapping.cpp
    src/WNMemoryBackedMapping.cpp
    src/WNMemoryBackedMapping.h
    src/WNUtilities.h
  LIBS
    WNUtilities
)

add_subdirectory(test)
