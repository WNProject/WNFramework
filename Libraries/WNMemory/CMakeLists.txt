set(INC_INTERNAL
  inc/Internal/WNConfig.h
  inc/Internal/WNPrefetch.h
)

source_group("inc\\Internal" FILES ${INC_INTERNAL})
add_wn_header_library(WNMemory
  inc/WNAllocation.h
  inc/WNAllocator.h
  inc/WNConfig.h
  inc/WNIntrusivePtr.h
  inc/WNIntrusivePtrBase.h
  inc/WNManipulation.h
  inc/WNMemory.h
  inc/WNPrefetch.h
  ${INC_INTERNAL}
)

add_subdirectory(test)
