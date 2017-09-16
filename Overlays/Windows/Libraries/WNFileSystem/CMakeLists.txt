overlay_library_sources(WNFileSystem
  SOURCES
    src/WNFileWindows.cpp
    src/WNFileWindows.h
    src/WNMappingWindows.cpp
    src/WNMappingWindows.h
    src/WNSystemPaths.cpp
    src/WNSystemPaths.h
  LIBS
    Rpcrt4.lib
)

overlay_set_variable(WN_FILE_SYSTEM_MAPPING_H
  "WNFileSystem/src/WNMappingWindows.h")

overlay_set_variable(WN_FILE_SYSTEM_SYSTEM_MAPPING_TYPE
  "mapping_windows")