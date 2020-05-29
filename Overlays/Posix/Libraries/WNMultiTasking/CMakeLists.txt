set(CMAKE_REQUIRED_LIBRARIES pthread)

overlay_library_sources(
  WNMultiTasking
  SOURCES
    inc/internal/mutex_base.h
    inc/internal/shared_mutex_base.h
    inc/internal/system_thread_yield.h
    inc/internal/thread_base.h
    inc/internal/utilities.h
)
