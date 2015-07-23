set(INC_INTERNAL
  inc/Internal/WNBase_Clang.h
  inc/Internal/WNBase_Clang_GCC.h
  inc/Internal/WNBase_GCC.h
  inc/Internal/WNBase_MSVC.h
  inc/Internal/WNConfig.h
  inc/Internal/WNErrors.inc
  inc/Internal/WNExtendedTypes.h
)
source_group("inc\\Internal" FILES ${INC_INTERNAL})
set(INC_INTERNAL_ARM
  inc/Internal/ARM/WNBase.h
  inc/Internal/ARM/WNBase_Clang.h
  inc/Internal/ARM/WNBase_GCC.h
  inc/Internal/ARM/WNBase_MSVC.h
  inc/Internal/ARM/WNConfig.h
)
source_group("inc\\Internal\\ARM" FILES ${INC_INTERNAL_ARM})
set(INC_INTERNAL_X86
  inc/Internal/X86/WNBase.h
  inc/Internal/X86/WNBase_Clang.h
  inc/Internal/X86/WNBase_GCC.h
  inc/Internal/X86/WNBase_MSVC.h
  inc/Internal/X86/WNConfig.h
)
source_group("inc\\Internal\\x86" FILES ${INC_INTERNAL_X86})
add_wn_header_library(WNCore
  inc/WNAlgorithm.h
  inc/WNAssert.h
  inc/WNBase.h
  inc/WNConfig.h
  inc/WNCore.h
  inc/WNEndian.h
  inc/WNExtendedTypes.h
  inc/WNTypes.h
  inc/WNTypeTraits.h
  inc/WNUtility.h
  ${INC_INTERNAL}
  ${INC_INTERNAL_ARM}
  ${INC_INTERNAL_X86}
)

add_subdirectory(test)

