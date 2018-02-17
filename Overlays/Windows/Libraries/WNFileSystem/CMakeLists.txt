overlay_library_sources(WNFileSystem
  SOURCES
    src/WNFileWindows.cpp
    src/WNFileWindows.h
    src/WNHelpers.h
    src/WNMappingWindows.cpp
    src/WNMappingWindows.h
    src/WNSystemMapping.h
    src/WNSystemPaths.cpp
    src/WNSystemUtilities.h
  LIBS
    Rpcrt4.lib
)

overlay_message(STATUS "File System Mapping Type: Windows")
