enable_overlay_directory()

add_wn_header_library(WNMemory
  SOURCES
    inc/allocation_tracker.h
    inc/allocation.h
    inc/allocator.h
    inc/basic_allocator.h
    inc/WNIntrusivePtr.h
    inc/manipulation.h
    inc/WNSTLCompatibleAllocator.h
    inc/WNStringUtility.h
    inc/WNUniquePtr.h
)

wn_add_subdirectory(test)

post_enable_overlay_directory()