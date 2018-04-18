enable_overlay_directory()

add_wn_header_library(WNMemory
  SOURCES
    inc/allocation_tracker.h
    inc/allocation.h
    inc/allocator.h
    inc/basic_allocator.h
    inc/intrusive_ptr.h
    inc/manipulation.h
    inc/stl_allocator.h
    inc/string_utility.h
    inc/unique_ptr.h
)

wn_add_subdirectory(test)

post_enable_overlay_directory()
