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

source_group("inc\\Internal\\Extension\\x86\\3DNow" FILES ${INC_INTERNAL_EXTENSION_X86_3DNOW})
source_group("inc\\Internal\\Extension\\x86\\SSE" FILES ${INC_INTERNAL_EXTENSION_X86_SSE})
source_group("inc\\Internal\\Extension\\x86" FILES ${INC_INTERNAL_EXTENSION_X86})
source_group("inc\\Internal\\Extension" FILES ${INC_INTERNAL_EXTENSION})
source_group("inc\\Internal\\Generic" FILES ${INC_INTERNAL_GENERIC})
source_group("inc\\Internal" FILES ${INC_INTERNAL})

add_wn_header_library(WNMemory
  ${INC_INTERNAL_EXTENSION_X86_3DNOW}
  ${INC_INTERNAL_EXTENSION_X86_SSE}
  ${INC_INTERNAL_EXTENSION_X86}
  ${INC_INTERNAL_EXTENSION}
  ${INC_INTERNAL_GENERIC}
  ${INC_INTERNAL}
  inc/WNAllocator.h
  inc/WNBasic.h
  inc/WNConfig.h
  inc/WNIntrusivePtr.h
  inc/WNStringUtility.h
  inc/WNUniquePtr.h
)

add_subdirectory(test)