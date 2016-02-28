add_wn_header_library(WNLogging
  inc/Internal/WNBufferLogger.inl
  inc/Internal/WNConsoleLoggerAndroid.inl
  inc/Internal/WNConsoleLoggerLinux.inl
  inc/Internal/WNConsoleLoggerWindows.inl
  inc/Internal/WNMultiLogger.inl
  inc/WNBufferLogger.h
  inc/WNConsoleLogger.h
  inc/WNDefaultLogTypes.inl
  inc/WNFileLogger.h
  inc/WNLog.h
  inc/WNLog.inl
  inc/WNMultiLogger.h
)

add_subdirectory(test)
