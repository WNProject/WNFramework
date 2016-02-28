set(INC_INTERNAL_EXTENSION_X86_3DNOW
  inc/Internal/Extension/x86/3DNow/WNBasicTraits.h
)

set(INC_INTERNAL_EXTENSION_X86_SSE
  inc/Internal/Extension/x86/SSE/WNBasicTraits.h
)

set(INC_INTERNAL_EXTENSION_X86
  inc/Internal/Extension/x86/WNBasicTraits.h
)

set(INC_INTERNAL_EXTENSION
  inc/Internal/Extension/WNBasicTraits.h
)

set(INC_INTERNAL_GENERIC
  inc/Internal/Generic/WNBasicTraits.h
)

set(INC_INTERNAL
  inc/Internal/WNConfig.h
  inc/Internal/WNBasicTraits.h
)

set(WN_MEMORY_FILES
  ${INC_INTERNAL_EXTENSION_X86_3DNOW}
  ${INC_INTERNAL_EXTENSION_X86_SSE}
  ${INC_INTERNAL_EXTENSION_X86}
  ${INC_INTERNAL_EXTENSION}
  ${INC_INTERNAL_GENERIC}
  ${INC_INTERNAL}
)

if (MSVC)
  set(NATVIS_FILES
    natvis/WNIntrusivePtr.natvis
    natvis/WNUniquePtr.natvis
  )

  list(APPEND WN_MEMORY_FILES ${NATVIS_FILES})
endif()

add_wn_header_library(WNMemory
  ${WN_MEMORY_FILES}
  inc/WNAllocator.h
  inc/WNBasic.h
  inc/WNConfig.h
  inc/WNIntrusivePtr.h
  inc/WNSTLCompatibleAllocator.h
  inc/WNStringUtility.h
  inc/WNUniquePtr.h
)

add_subdirectory(test)