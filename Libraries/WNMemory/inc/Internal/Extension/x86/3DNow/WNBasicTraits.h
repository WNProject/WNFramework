#pragma once

#ifndef __WN_MEMORY_INTERNAL_EXTENSION_X86_3DNOW_BASIC_TRAITS_H__
#define __WN_MEMORY_INTERNAL_EXTENSION_X86_3DNOW_BASIC_TRAITS_H__

#include "WNMemory/inc/Internal/Generic/WNBasicTraits.h"

#include <mm3dnow.h>

namespace wn {
namespace memory {
namespace internal {

struct basic_traits_3dnow : basic_traits_generic {
  template <typename T>
  static WN_FORCE_INLINE wn_void prefetch(const T* ptr) {
    _m_prefetchw(reinterpret_cast<const void*>(ptr));
  }
};

} // namespace internal
} // namespace memory
} // namespace wn

#endif // __WN_MEMORY_INTERNAL_EXTENSION_X86_3DNOW_BASIC_TRAITS_H__