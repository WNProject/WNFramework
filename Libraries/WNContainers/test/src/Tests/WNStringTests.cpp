// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNTesting/inc/WNTestHarness.h"

// Since wn::containers::string is just a typedef for a specific
// type of std::basic_string, we avoid testing all of the basic_string
// operators. We do however want to make sure that all of the allocators
// are behaving correctly.

TEST(string, construction) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s(
        "this is a very long string that will not get optimized away",
        &allocator);

    EXPECT_EQ(
        std::string(
            "this is a very long string that will not get optimized away"),
        std::string(s.c_str()));
  }
}

TEST(string, allocator) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s(
        "this is a very long string that will not get optimized away",
        &allocator);
  }
}

TEST(string, allocator_copies) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s(
        "this is a very long string that will not get optimized away",
        &allocator);
    wn::containers::string s2(s);

    // The string implementation in gnustl does CoW. This means it is not
    // conformant to c++11.  We have to FORCE the copy to actually happen in
    // order to make sure that the correct allocator is used.
    s[0] = 'f';
  }
}

TEST(string, allocator_moves) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s(
        "this is a very long string that will not get optimized away",
        &allocator);
    wn::containers::string s2(std::move(s));

    EXPECT_EQ(s.size(), 0);
  }
}

TEST(string, allocator_swap) {
  wn::testing::allocator allocator1;
  wn::testing::allocator allocator2;

  {
    wn::containers::string s(
        "this is a very long string that will not get optimized away",
        &allocator1);
    wn::containers::string s2(
        "this is another very long string that will not get optimize daway",
        &allocator2);

    wn::core::swap(s, s2);
  }

  {
    wn::containers::string s(
        "this is a very long string that will not get optimized away",
        &allocator1);
    wn::containers::string s2(
        "this is another very long string that will not get optimize daway",
        &allocator1);

    wn::core::swap(s, s2);
  }
}

TEST(string, uses_correct_allocator_on_assignment) {
  wn::testing::allocator allocator1;
  wn::testing::allocator allocator2;

  {
    wn::containers::string s(
        "this is a very long string that will not get optimized away",
        &allocator1);
    wn::containers::string s2(&allocator2);

    s2 = s;
  }
}
