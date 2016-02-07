// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_UTILITIES_H__
#define __WN_MATH_INTERNAL_X86_UTILITIES_H__

#include "WNCore/inc/WNTypeTraits.h"

#include <immintrin.h>

#define __WN_XMM_CAST_PS_TO_SI128(_value) *reinterpret_cast<__m128i*>(&_value)

namespace wn {
namespace internal {
namespace math {
template <const size_t _dimension>
using mm_count_pi32 = core::index_constant<_dimension / 2>;

template <const size_t _dimension>
using mm_count_ps = core::index_constant<(_dimension + 1) / 2>;

template <const size_t _dimension>
using xmm_count_pi32 =
    core::index_constant<(_dimension % 4) == 1 ? (_dimension / 4)
                                               : ((_dimension + 3) / 4)>;

template <const size_t _dimension>
using xmm_count_pi64 =
    core::index_constant<(_dimension % 2) == 1 ? (_dimension / 2)
                                               : ((_dimension + 1) / 2)>;

template <const size_t _dimension>
using xmm_count_ps = core::index_constant<(_dimension + 3) / 4>;

template <const size_t _dimension>
using xmm_count_pd = core::index_constant<(_dimension + 1) / 2>;

template <uint32_t dimension>
struct __WNNumberRegisters_YMM_PI32 {
  enum {
    Value = (dimension % 8) >= 5 ? ((dimension + 7) / 8) : (dimension / 8)
  };
};

template <uint32_t dimension>
struct __WNNumberRegisters_YMM_PI64 {
  enum {
    Value = (dimension % 4) >= 3 ? ((dimension + 3) / 4) : (dimension / 4)
  };
};

template <uint32_t dimension>
struct __WNNumberRegisters_YMM_PS {
  enum {
    Value = (dimension % 8) >= 5 ? ((dimension + 7) / 8) : (dimension / 8)
  };
};

template <uint32_t dimension>
struct __WNNumberRegisters_YMM_PD {
  enum {
    Value = (dimension % 4) >= 3 ? ((dimension + 3) / 4) : (dimension / 4)
  };
};
}
}
};

#ifdef __AVX_AVAILABLE
WN_FORCE_INLINE void avx_zero_upper() {
#ifndef __WN_VEX_AVAILABLE
  _mm256_zeroupper();
#endif
}
#endif  // __AVX_AVAILABLE

#endif  // __WN_MATH_INTERNAL_X86_UTILITIES_H__
