// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMemory/test/inc/Common.h"
#include "WNCore/inc/WNExtendedTypes.h"

template <typename _Type>
struct prefetch : ::testing::Test {};

typedef ::testing::Types<wn_uint32, wn_int32, wn_float32, wn_fixed32> prefetch_testing_types;

TYPED_TEST_CASE(prefetch, prefetch_testing_types);

TYPED_TEST(prefetch, usage) {
    TypeParam value = static_cast<TypeParam>(0);

    wn::memory::prefetch(&value);

    TypeParam values[4] = { static_cast<TypeParam>(0) };

    wn::memory::prefetch(&values, 2);

    const wn_void* void_value = values;

    wn::memory::prefetch(&void_value, sizeof(TypeParam) * 3);
}