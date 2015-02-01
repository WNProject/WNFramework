// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_EXTENDED_TYPE_DECLARES_H__
#define __WN_CORE_INTERNAL_EXTENDED_TYPE_DECLARES_H__

#include "WNCore/inc/WNTypes.h"

namespace wn {
    namespace internal {
        template <typename _traits_type>
        struct arithmetic_type;

        template <typename _type, const wn_uint8 _precision>
        struct fixed_point_traits;

        template <typename _type, typename _conversion_type>
        struct floating_point_traits;

        struct float8_conversions;
        struct float16_conversions;
    }
}

typedef wn::internal::arithmetic_type<wn::internal::fixed_point_traits<wn_int8, 4>> wn_fixed8;
typedef wn::internal::arithmetic_type<wn::internal::fixed_point_traits<wn_int16, 8>> wn_fixed16;
typedef wn::internal::arithmetic_type<wn::internal::fixed_point_traits<wn_int32, 16>> wn_fixed32;
typedef wn::internal::arithmetic_type<wn::internal::fixed_point_traits<wn_int64, 32>> wn_fixed64;

template <typename _type, const wn_uint8 _precision>
using wn_fixed = wn::internal::arithmetic_type<wn::internal::fixed_point_traits<_type, _precision>>;

typedef wn::internal::arithmetic_type<wn::internal::floating_point_traits<wn_uint8,
                                                                          wn::internal::float8_conversions>> wn_float8;
typedef wn::internal::arithmetic_type<wn::internal::floating_point_traits<wn_uint16,
                                                                          wn::internal::float16_conversions>> wn_float16;

#endif // __WN_CORE_INTERNAL_EXTENDED_TYPE_DECLARES_H__