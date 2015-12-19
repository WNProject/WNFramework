// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNTesting/inc/WNTestHarness.h"
#include "WNContainers/inc/WNString.h"

// Since wn::containers::string is just a typedef for a specific
// type of std::basic_string, we avoid testing all of the basic_string
// operators. We do however want to make sure that all of the allocators
// are behaving correctly.

TEST(string, construction) {
  // Global state in these tests!!! :(
  // Specifically these tests cannot be threaded. This is because
  // get_default_allocator() is the backup iterator if none is provided.
  // We  want to make sure that it IS used when none is provided,
  // but not used when one is not provided.
  // This specifically means that it will be inconsistent with expectations
  // if we multithread.
  auto* def = wn::containers::wn_string_test_allocator::get_default_allocator();
  // In Release string has some small-string optimizations (hooray),
  // but we will make sure that we use large strings to actually
  // test the allocators.
  wn::containers::test_string s(
      "this is a very long string that will not get optimized away");
  EXPECT_EQ(std::string(
                "this is a very long string that will not get optimized away"),
            std::string(s.c_str()));
  EXPECT_GT(def->allocated(), 0u);
}

TEST(string, allocator) {
  auto* def = wn::containers::wn_string_test_allocator::get_default_allocator();
  wn_size_t global_allocated = def->allocated();
  wn::memory::default_test_allocator test;
  {
    wn::containers::test_string s(
        "this is a very long string that will not get optimized away", &test);
    EXPECT_GT(test.allocated(), 0u);
  }
  EXPECT_EQ(test.freed(), test.allocated());

  // Nothing should have gone through the default allocator.
  EXPECT_EQ(global_allocated, def->allocated());
}

TEST(string, allocator_copies) {
  auto* def = wn::containers::wn_string_test_allocator::get_default_allocator();
  wn_size_t global_allocated = def->allocated();
  wn::memory::default_test_allocator test;
  {
    wn::containers::test_string s(
        "this is a very long string that will not get optimized away", &test);
    EXPECT_GT(test.allocated(), 0u);
    wn_size_t allocated = test.allocated();
    wn::containers::test_string s2(s);
    // The string implementation in gnustl implementation is not
    // conformant to c++11. This means that is does CoW.
    // We have to FORCE the copy to actually happen in order to
    // make sure that the correct allocator is used.
    s[0] = 'f';
    EXPECT_GE(test.allocated(), allocated);
  }
  EXPECT_EQ(test.freed(), test.allocated());

  // Nothing should have gone through the default allocator.
  EXPECT_EQ(global_allocated, def->allocated());
}

TEST(string, allocator_moves) {
  auto* def = wn::containers::wn_string_test_allocator::get_default_allocator();
  wn_size_t global_allocated = def->allocated();
  wn::memory::default_test_allocator test;
  {
    wn::containers::test_string s(
        "this is a very long string that will not get optimized away", &test);
    EXPECT_GT(test.allocated(), 0u);
    wn_size_t allocated = test.allocated();
    wn::containers::test_string s2(std::move(s));
#ifdef _NDEBUG
    EXPECT_EQ(test.allocated(), allocated);
#else
    // In debug std::string, at least on some platforms,
    // allocates an extra std::pair to hold the char*.
    // This means that even on move operations some memory
    // gets allocated. We make sure that the ENTIRE string was
    // not allocated again.
    EXPECT_LT(test.allocated(), allocated * 2);
#endif
    EXPECT_EQ(0u, s.size());
  }
  EXPECT_EQ(test.freed(), test.allocated());

  // Nothing should have gone through the default allocator.
  EXPECT_EQ(global_allocated, def->allocated());
}

TEST(string, allocator_swap) {
  auto* def = wn::containers::wn_string_test_allocator::get_default_allocator();
  wn_size_t global_allocated = def->allocated();
  wn::memory::default_test_allocator test;
  wn::memory::default_test_allocator test2;
  {
    wn::containers::test_string s(
        "this is a very long string that will not get optimized away", &test);
    EXPECT_GT(test.allocated(), 0u);
    wn::containers::test_string s2(
        "this is another very long string that will not get optimize daway",
        &test2);
    wn_size_t alloc1 = test.allocated();
    wn_size_t alloc2 = test2.allocated();
    EXPECT_GT(alloc1, 0u);
    EXPECT_GT(alloc2, 0u);
    std::swap(s, s2);
    // Based on how much memory was actually allocated for
    // s1 and s2 there does not necessarily have to be
    // any new memory allocated, although it is likely at least
    // one is.
    EXPECT_GE(test.allocated(), alloc1);
    EXPECT_GE(test2.allocated(), alloc2);
  }
  {
    wn::containers::test_string s(
        "this is a very long string that will not get optimized away", &test);
    EXPECT_GT(test.allocated(), 0u);
    wn::containers::test_string s2(
        "this is another very long string that will not get optimize daway",
        &test);
    wn_size_t alloc1 = test.allocated();
    EXPECT_GT(alloc1, 0u);
    std::swap(s, s2);
    // Since they were allocated with the same allocator, we
    // require that no extra memory was allocated.
    EXPECT_EQ(alloc1, test.allocated());
  }

  EXPECT_EQ(test.freed(), test.allocated());

  // Nothing should have gone through the default allocator.
  EXPECT_EQ(global_allocated, def->allocated());
}

TEST(string, uses_correct_allocator_on_assignment) {
  auto* def = wn::containers::wn_string_test_allocator::get_default_allocator();
  wn_size_t global_allocated = def->allocated();
  wn::memory::default_test_allocator test;
  wn::memory::default_test_allocator test2;
  {
    wn::containers::test_string s(
        "this is a very long string that will not get optimized away", &test);
    EXPECT_GT(test.allocated(), 0u);
    wn_size_t alloc1 = test.allocated();

    // The second string is made with a different allocator.
    wn::containers::test_string s2(&test2);
    s2 = s;

    // On assignment the new memory should have come out of the correct
    // allocator.
    EXPECT_GT(test2.allocated(), 0u);
    wn_size_t alloc2 = test2.allocated();
    // The original allocator should not have done anything.
    EXPECT_EQ(alloc1, test.allocated());
    // The new allocator should have allocated at least enough for the string.
    EXPECT_GE(alloc2, s2.size());
  }

  EXPECT_EQ(test.freed(), test.allocated());
  EXPECT_EQ(test2.freed(), test2.allocated());

  // Nothing should have gone through the default allocator.
  EXPECT_EQ(global_allocated, def->allocated());
}
