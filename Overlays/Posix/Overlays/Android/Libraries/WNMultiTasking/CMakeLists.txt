overlay_library_sources(
  WNMultiTasking
  SOURCES
    inc/internal/system_thread_functions.h
    inc/internal/system_thread_yield.h
    src/context.h
    src/fiber_entry.h
    src/fiber_entry.cpp
    src/swap_context.cpp
)

wn_add_subdirectory(test)
