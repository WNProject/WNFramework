enable_overlay_directory()

add_wn_header_library(WNLogging
  SOURCES
    inc/Internal/WNBufferLogger.inl
    inc/Internal/WNMultiLogger.inl
    inc/WNBufferLogger.h
    inc/WNConsoleLogger.h
    inc/WNDefaultLogTypes.inl
    inc/WNFileLogger.h
    inc/WNLog.h
    inc/WNLog.inl
    inc/WNMultiLogger.h
  LIBS
)

add_subdirectory(test)
