overlay_library_sources(WNMultiTasking
  SOURCES
    src/context.h
    src/fiber_entry.h
    src/fiber_entry.cpp
    src/swap_context.cpp
)

wn_add_subdirectory(test)
