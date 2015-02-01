// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNCore/test/inc/Common.h"

TEST(extended_types, wn_fixed8_creation_standard) {

}

TEST(extended_types, wn_fixed8_assignment_standard) {

}

TEST(extended_types, wn_fixed8_negation_standard) {

}

TEST(extended_types, wn_fixed8_addition_standard) {
    wn_fixed8 fixed = 0.25f;

    ASSERT_EQ(fixed.representation, 0x04); // 0.25

    fixed += fixed;

    ASSERT_EQ(fixed.representation, 0x08); // 0.5

    fixed = fixed + fixed;

    ASSERT_EQ(fixed.representation, 0x10); // 1.0

    fixed += 1;

    ASSERT_EQ(fixed.representation, 0x20); // 2.0

    fixed = fixed + 1;

    ASSERT_EQ(fixed.representation, 0x30); // 3.0

    fixed = 1 + fixed;

    ASSERT_EQ(fixed.representation, 0x40); // 4.0

    fixed += 0.25f;

    ASSERT_EQ(fixed.representation, 0x44); // 4.25

    fixed = fixed + 0.25f;

    ASSERT_EQ(fixed.representation, 0x48); // 4.5

    fixed = 0.25f + fixed;

    ASSERT_EQ(fixed.representation, 0x4C); // 4.75
}

TEST(extended_types, wn_fixed8_subtraction_standard) {
    wn_fixed8 fixed = 4.725f;

    fixed -= fixed;

    ASSERT_EQ(fixed.representation, 0x00); // 0.0

    fixed = fixed - fixed;

    ASSERT_EQ(fixed.representation, 0x00); // 0.0

    fixed -= 1;

    ASSERT_EQ(fixed.representation, -0x10); // -1.0

    fixed = fixed - 1;

    ASSERT_EQ(fixed.representation, -0x20); // -2.0

    fixed = 1 - fixed;

    ASSERT_EQ(fixed.representation, 0x30); // 3.0

    fixed -= 0.25f;

    ASSERT_EQ(fixed.representation, 0x2C); // 2.75

    fixed = fixed - 0.25f;

    ASSERT_EQ(fixed.representation, 0x28); // 2.5

    fixed = 0.25f - fixed;

    ASSERT_EQ(fixed.representation, -0x24); // -2.25
}

TEST(extended_types, wn_fixed8_multiplication_standard) {

}

TEST(extended_types, wn_fixed8_division_standard) {

}

TEST(extended_types, wn_fixed8_comparators_standard) {

}

TEST(extended_types, wn_fixed8_usage_standard) {

}