// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/test/inc/Common.h"

template <typename _Type>
struct dynamic_array : ::testing::Test {};

typedef ::testing::Types<wn_uint8, wn_uint16, wn_uint32, wn_uint64> dynamic_array_testing_types;

TYPED_TEST_CASE(dynamic_array, dynamic_array_testing_types);