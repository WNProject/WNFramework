set(INC_INTERNAL
  inc/Internal/WNBufferLogger.inl
  inc/Internal/WNConsoleLoggerAndroid.inl
  inc/Internal/WNConsoleLoggerLinux.inl
  inc/Internal/WNConsoleLoggerWindows.inl
  inc/Internal/WNMultiLogger.inl
)

source_group("inc\\Internal" FILES ${INC_INTERNAL})
add_wn_header_library(WNLogging
  inc/WNBufferLogger.h
  inc/WNConsoleLogger.h
  inc/WNDefaultLogTypes.inl
  inc/WNFileLogger.h
  inc/WNLog.h
  inc/WNLog.inl
  inc/WNMultiLogger.h
  ${INC_INTERNAL}
)

add_subdirectory(test)