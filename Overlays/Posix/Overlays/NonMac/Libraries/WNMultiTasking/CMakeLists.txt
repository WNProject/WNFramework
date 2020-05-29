set(CMAKE_REQUIRED_LIBRARIES pthread)

overlay_library_sources(
  WNMultiTasking
  SOURCES
    inc/internal/semaphore_base.h
)
