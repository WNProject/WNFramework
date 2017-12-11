set(CMAKE_REQUIRED_LIBRARIES pthread)

overlay_library_sources(
  WNMultiTasking
  SOURCES
    inc/internal/mutex_base.h
    inc/internal/semaphore_base.h
    inc/internal/shared_mutex_base.h
)
