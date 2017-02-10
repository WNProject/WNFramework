enable_overlay_directory()

if (NOT WN_MAX_LOG_LEVEL)
  set(WN_MAX_LOG_LEVEL "max")
endif()

SET(WN_MAX_LOG_LEVEL ${WN_MAX_LOG_LEVEL} CACHE STRING
  "Maximum log level that normal loggers will emit")

set_property(CACHE WN_MAX_LOG_LEVEL PROPERTY STRINGS none critical error warning issue info debug max)

message(STATUS "Default Maximum Log Level: ${WN_MAX_LOG_LEVEL}")

add_wn_header_library(WNLogging
  SOURCES
    inc/Internal/WNBufferLogger.inl
    inc/Internal/WNConfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNConfig.h
    inc/Internal/WNMultiLogger.inl
    inc/WNBufferLogger.h
    inc/WNConsoleLogger.h
    inc/WNDefaultLogTypes.inl
    inc/WNFileLogger.h
    inc/WNLog.h
    inc/WNLog.inl
    inc/WNLogEnums.h
    inc/WNMultiLogger.h
  LIBS
)

add_subdirectory(test)

configure_file(inc/Internal/WNConfig.h.in
  inc/Internal/WNConfig.h)