enable_overlay_directory()

set(WN_ENABLED_MEMORY_EXTENSIONS "_WN_MEMORY_EXTENSIONS_DISABLED")
if(WN_MEMORY_TRAITS_NAME)
  set(WN_HAS_MEMORY_EXTENSIONS "_WN_MEMORY_EXTENSIONS_ENABLED")
endif()

configure_file(inc/Internal/WNBasicTraits.h.in
  inc/Internal/WNBasicTraits.h)

add_wn_header_library(WNMemory
  SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNBasicTraits.h
    inc/WNAllocator.h
    inc/WNBasic.h
    inc/WNConfig.h
    inc/WNIntrusivePtr.h
    inc/WNSTLCompatibleAllocator.h
    inc/WNStringUtility.h
    inc/WNUniquePtr.h
)

wn_add_subdirectory(test)

post_enable_overlay_directory()