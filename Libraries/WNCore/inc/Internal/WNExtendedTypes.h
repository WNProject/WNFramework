// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_EXTENDED_TYPE_DECLARES_H__
#define __WN_CORE_INTERNAL_EXTENDED_TYPE_DECLARES_H__

#include "WNCore/inc/WNTypes.h"

namespace wn {
    namespace core {
        namespace internal {
            template <typename _TraitsType>
            struct arithmetic_type;

            template <typename _Type, const wn_uint8 _Precision>
            struct fixed_point_traits;

            template <typename _Type, typename _ConversionType>
            struct floating_point_traits;

            struct float8_conversions;
            struct float16_conversions;
        }
    }
}

typedef wn::core::internal::arithmetic_type<wn::core::internal::fixed_point_traits<wn_int8, 4>> wn_fixed8;
typedef wn::core::internal::arithmetic_type<wn::core::internal::fixed_point_traits<wn_int16, 8>> wn_fixed16;
typedef wn::core::internal::arithmetic_type<wn::core::internal::fixed_point_traits<wn_int32, 16>> wn_fixed32;
typedef wn::core::internal::arithmetic_type<wn::core::internal::fixed_point_traits<wn_int64, 32>> wn_fixed64;

template <typename _Type, const wn_uint8 _Precision>
using wn_fixed = wn::core::internal::arithmetic_type<wn::core::internal::fixed_point_traits<_Type, _Precision>>;

typedef wn::core::internal::arithmetic_type<wn::core::internal::floating_point_traits<wn_uint8,
                                                                                      wn::core::internal::float8_conversions>> wn_float8;
typedef wn::core::internal::arithmetic_type<wn::core::internal::floating_point_traits<wn_uint16,
                                                                                      wn::core::internal::float16_conversions>> wn_float16;

#endif // __WN_CORE_INTERNAL_EXTENDED_TYPE_DECLARES_H__
