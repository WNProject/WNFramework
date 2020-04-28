// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNRangePartition.h"
#include "executable_test/inc/WNTestHarness.h"

using range_partition_tests = ::testing::TestWithParam<size_t>;

TEST_P(range_partition_tests, insert) {
  wn::testing::allocator m_allocator;
  using range_partition = wn::containers::range_partition<>;
  range_partition l(&m_allocator, GetParam());

  range_partition::token token = l.get_interval(GetParam() / 2);
  EXPECT_TRUE(token.is_valid());

  range_partition::token token2 = l.get_interval(GetParam() / 4);
  EXPECT_TRUE(token2.is_valid());
}

TEST_P(range_partition_tests, allocate_all) {
  wn::testing::allocator m_allocator;
  using range_partition = wn::containers::range_partition<>;
  range_partition l(&m_allocator, GetParam());

  range_partition::token token = l.get_interval(GetParam() / 2);
  EXPECT_TRUE(token.is_valid());

  range_partition::token token2 = l.get_interval(l.size() - l.used());
  EXPECT_TRUE(token2.is_valid());

  range_partition::token token3 = l.get_interval(1);
  EXPECT_FALSE(token3.is_valid());
}

TEST_P(range_partition_tests, insert_then_allocate_all) {
  wn::testing::allocator m_allocator;
  using range_partition = wn::containers::range_partition<>;
  range_partition l(&m_allocator, GetParam());

  range_partition::token token = l.get_interval(GetParam() / 2);
  EXPECT_TRUE(token.is_valid());

  range_partition::token token2 = l.get_interval(GetParam() / 4);
  EXPECT_TRUE(token2.is_valid());

  token.free();
  token2.free();

  token = l.get_interval(GetParam());
  EXPECT_TRUE(token.is_valid());
}

TEST_P(range_partition_tests, allocate_all_by_1) {
  wn::testing::allocator m_allocator;
  using range_partition = wn::containers::range_partition<>;
  range_partition l(&m_allocator, GetParam());
  wn::containers::dynamic_array<range_partition::token> tokens(&m_allocator);
  tokens.reserve(GetParam());
  for (size_t i = 0; i < GetParam(); ++i) {
    tokens.emplace_back(l.get_interval(1));
    EXPECT_TRUE(tokens.back().is_valid());
  }

  range_partition::token token = l.get_interval(1);
  EXPECT_FALSE(token.is_valid());
}

INSTANTIATE_TEST_SUITE_P(various_sizes, range_partition_tests,
    ::testing::Values(10, 32, 41, 64, 129, 2048));

TEST_P(range_partition_tests, aligned) {
  wn::testing::allocator m_allocator;
  using range_partition = wn::containers::range_partition<>;
  range_partition l(&m_allocator, 1024 * 1024);
  wn::containers::dynamic_array<range_partition::token> tokens(&m_allocator);
  tokens.reserve(10);

  for (size_t i = 0; i < 10; ++i) {
    tokens.emplace_back(
        l.get_aligned_interval(i + 1, static_cast<size_t>(1) << i));
    EXPECT_TRUE(tokens.back().is_valid());
    EXPECT_TRUE((tokens.back().offset() % (static_cast<size_t>(1) << i)) == 0);
  }
}

TEST_P(range_partition_tests, small_aligned) {
  wn::testing::allocator m_allocator;
  using range_partition = wn::containers::range_partition<
      wn::containers::default_node_allocator<uint64_t>, uint64_t>;
  range_partition l(&m_allocator, 16);

  auto tok0 = l.get_aligned_interval(1, 1);
  auto tok1 = l.get_aligned_interval(1, 4);
  auto tok2 = l.get_aligned_interval(1, 8);
  EXPECT_TRUE(tok0.is_valid());
  EXPECT_TRUE(tok1.is_valid());
  EXPECT_TRUE(tok2.is_valid());
  EXPECT_TRUE((tok0.offset() % 1) == 0);
  EXPECT_TRUE((tok0.offset() % 4) == 0);
  EXPECT_TRUE((tok0.offset() % 8) == 0);
}

TEST_P(range_partition_tests, aligned_release) {
  wn::testing::allocator m_allocator;
  using range_partition = wn::containers::range_partition<>;
  range_partition l(&m_allocator, 1024 * 1024);

  auto tok0 = l.get_aligned_interval(1, 1);
  auto tok1 = l.get_aligned_interval(1, 4);
  auto offs = tok1.offset();
  auto tok2 = l.get_aligned_interval(1, 8);
  EXPECT_TRUE(tok0.is_valid());
  EXPECT_TRUE(tok1.is_valid());
  EXPECT_TRUE(tok2.is_valid());
  tok1.free();
  EXPECT_EQ(2u, l.used());
  tok1 = l.get_aligned_interval(1, 4);
  EXPECT_EQ(offs, tok1.offset());
}
