// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/test/inc/Common.h"
#include "WNContainers/inc/WNContiguousRange.h"

template <typename _Type>
struct contiguous_range : ::testing::Test {};

// 72 bit struct
struct dummy {
  WN_FORCE_INLINE dummy(const wn_uint8 value) :
    m_value1(value),
    m_value2(value) {
  }

  wn_uint8 m_value1;
  wn_uint64 m_value2;
};

WN_FORCE_INLINE wn_bool operator == (const dummy& dummy1,
                                     const dummy& dummy2) {
  return(dummy1.m_value1 == dummy2.m_value1 &&
         dummy1.m_value2 == dummy2.m_value2);
}

typedef ::testing::Types<wn_uint8, wn_uint16, wn_uint32, wn_uint64, dummy>
  contiguous_range_testing_types;

TYPED_TEST_CASE(contiguous_range, contiguous_range_testing_types);

TYPED_TEST(contiguous_range, construction) {
  TypeParam buffer[5] = { TypeParam(1), TypeParam(2),
                          TypeParam(3), TypeParam(4),
                          TypeParam(5) };
  wn::containers::contiguous_range<TypeParam> range1;
  wn::containers::contiguous_range<TypeParam> range2(wn_nullptr);
  wn::containers::contiguous_range<TypeParam> range3(wn_nullptr, wn_nullptr);
  wn::containers::contiguous_range<TypeParam> range4(buffer, buffer + 5);
  wn::containers::contiguous_range<const TypeParam> range5(buffer, 5);
  wn::containers::contiguous_range<TypeParam> range6(buffer);
  wn::containers::contiguous_range<TypeParam> range7(range6);

  EXPECT_TRUE(range1.empty());
  EXPECT_EQ(range1.data(), wn_nullptr);
  EXPECT_TRUE(range2.empty());
  EXPECT_EQ(range2.data(), wn_nullptr);
  EXPECT_TRUE(range3.empty());
  EXPECT_EQ(range3.data(), wn_nullptr);
  EXPECT_FALSE(range4.empty());
  EXPECT_EQ(range4.data(), buffer);
  EXPECT_FALSE(range5.empty());
  EXPECT_EQ(range5.data(), buffer);
  EXPECT_FALSE(range6.empty());
  EXPECT_EQ(range6.data(), buffer);
  EXPECT_FALSE(range7.empty());
  EXPECT_EQ(range7.data(), buffer);

  wn::containers::contiguous_range<const TypeParam> range8(std::move(range7));

  EXPECT_TRUE(range7.empty());
  EXPECT_EQ(range7.data(), wn_nullptr);
  EXPECT_FALSE(range8.empty());
  EXPECT_EQ(range8.data(), buffer);

  EXPECT_DEATH({
      const wn::containers::contiguous_range<TypeParam> range(wn_nullptr,
                                                              buffer + 5);
    },
    "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid input "
    "parameters, both must be null or non-null"
  );

  EXPECT_DEATH({
      const wn::containers::contiguous_range<TypeParam> range(buffer,
                                                              wn_nullptr);
    },
    "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid input "
    "parameters, both must be null or non-null"
  );
}

TYPED_TEST(contiguous_range, assignment) {
  TypeParam buffer[5] = { TypeParam(1), TypeParam(2),
                          TypeParam(3), TypeParam(4),
                          TypeParam(5) };
  wn::containers::contiguous_range<TypeParam> range1(buffer);

  range1 = wn_nullptr;

  wn::containers::contiguous_range<TypeParam> range2;

  range2 = buffer;

  wn::containers::contiguous_range<TypeParam> range3;

  range3 = range2;

  EXPECT_TRUE(range1.empty());
  EXPECT_EQ(range1.data(), wn_nullptr);
  EXPECT_FALSE(range2.empty());
  EXPECT_EQ(range2.data(), buffer);
  EXPECT_FALSE(range3.empty());
  EXPECT_EQ(range3.data(), buffer);

  wn::containers::contiguous_range<const TypeParam> range4;

  range4 = std::move(range3);

  EXPECT_TRUE(range3.empty());
  EXPECT_EQ(range3.data(), wn_nullptr);
  EXPECT_FALSE(range4.empty());
  EXPECT_EQ(range4.data(), buffer);
}

TYPED_TEST(contiguous_range, capacity) {
  TypeParam buffer[5] = { TypeParam(1), TypeParam(2),
                          TypeParam(3), TypeParam(4),
                          TypeParam(5) };
  const wn::containers::contiguous_range<TypeParam> range1(buffer);

  EXPECT_EQ(range1.size(), 5);
  EXPECT_EQ(range1.max_size(), 5);
  EXPECT_EQ(range1.max_size(), range1.size());

  const wn::containers::contiguous_range<TypeParam> range2(buffer + 1, 3);

  EXPECT_EQ(range2.size(), 3);
  EXPECT_EQ(range2.max_size(), 3);
  EXPECT_EQ(range2.max_size(), range2.size());
}

TYPED_TEST(contiguous_range, access) {
  TypeParam buffer[5] = { TypeParam(1), TypeParam(2),
                          TypeParam(3), TypeParam(4),
                          TypeParam(5) };
  const wn::containers::contiguous_range<TypeParam> range1(buffer);

  ASSERT_FALSE(range1.empty());
  EXPECT_EQ(range1.front(), TypeParam(1));
  EXPECT_EQ(range1.back(), TypeParam(5));
  EXPECT_EQ(range1.at(2), TypeParam(3));
  EXPECT_EQ(range1[2], TypeParam(3));
  EXPECT_EQ(range1.data(), buffer);

  const wn::containers::contiguous_range<TypeParam> range2(buffer + 1, 3);

  ASSERT_FALSE(range2.empty());
  EXPECT_EQ(range2.front(), TypeParam(2));
  EXPECT_EQ(range2.back(), TypeParam(4));
  EXPECT_EQ(range2.at(1), TypeParam(3));
  EXPECT_EQ(range2[1], TypeParam(3));
  EXPECT_EQ(range2.data(), buffer + 1);

  EXPECT_DEATH({
      const wn::containers::contiguous_range<TypeParam> range;

      range.at(0);
    },
    "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid contiguous range"
  );

  EXPECT_DEATH({
      const wn::containers::contiguous_range<TypeParam> range(buffer,
                                                              buffer + 5);

      range.at(5);
    },
    "assertion failed!\n\nfile: .*\nline: .*\nmessage: index out of bounds"
  );

  EXPECT_DEATH({
      const wn::containers::contiguous_range<TypeParam> range;

      range[0];
    },
    "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid contiguous range"
  );

  EXPECT_DEATH({
      const wn::containers::contiguous_range<TypeParam> range(buffer,
                                                              buffer + 5);

      range[5];
    },
    "assertion failed!\n\nfile: .*\nline: .*\nmessage: index out of bounds"
  );

  EXPECT_DEATH({
      const wn::containers::contiguous_range<TypeParam> range;

      range.front();
    },
    "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid contiguous range"
  );

  EXPECT_DEATH({
      const wn::containers::contiguous_range<TypeParam> range;

      range.back();
    },
    "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid contiguous range"
  );
}

TYPED_TEST(contiguous_range, interation) {
  TypeParam buffer[5] = { TypeParam(1), TypeParam(2),
                          TypeParam(3), TypeParam(4),
                          TypeParam(5) };
  wn::containers::contiguous_range<TypeParam> range1(buffer);

  wn_uint8 count = 1;

  for (auto i = range1.begin(); i != range1.end(); ++i) {
    EXPECT_EQ(*i, TypeParam(count));

    count++;
  }

  count = 1;

  for (auto i = range1.cbegin(); i != range1.cend(); ++i) {
    EXPECT_EQ(*i, TypeParam(count));

    count++;
  }

  count = 1;

  const wn::containers::contiguous_range<TypeParam> range2(range1);

  for (auto i = range2.begin(); i != range2.end(); ++i) {
    EXPECT_EQ(*i, TypeParam(count));

    count++;
  }

  count = 5;

  for (auto i = range1.rbegin(); i != range1.rend(); ++i) {
    EXPECT_EQ(*i, TypeParam(count));

    count--;
  }

  count = 5;

  for (auto i = range1.crbegin(); i != range1.crend(); ++i) {
    EXPECT_EQ(*i, TypeParam(count));

    count--;
  }

  count = 5;

  for (auto i = range2.rbegin(); i != range2.rend(); ++i) {
    EXPECT_EQ(*i, TypeParam(count));

    count--;
  }
}

TYPED_TEST(contiguous_range, swap) {
  TypeParam buffer1[5] = { TypeParam(1), TypeParam(2),
                           TypeParam(3), TypeParam(4),
                           TypeParam(5) };
  TypeParam buffer2[5] = { TypeParam(6), TypeParam(7),
                           TypeParam(8), TypeParam(9),
                           TypeParam(10) };
  wn::containers::contiguous_range<TypeParam> range1(buffer1);
  wn::containers::contiguous_range<TypeParam> range2(buffer2);

  EXPECT_FALSE(range1.empty());
  EXPECT_FALSE(range2.empty());
  ASSERT_EQ(range1.data(), buffer1);
  ASSERT_EQ(range2.data(), buffer2);
  EXPECT_EQ(range1.front(), TypeParam(1));
  EXPECT_EQ(range2.front(), TypeParam(6));
  EXPECT_EQ(range1.back(), TypeParam(5));
  EXPECT_EQ(range2.back(), TypeParam(10));

  range1.swap(range2);

  EXPECT_FALSE(range1.empty());
  EXPECT_FALSE(range2.empty());
  ASSERT_EQ(range1.data(), buffer2);
  ASSERT_EQ(range2.data(), buffer1);
  EXPECT_EQ(range1.front(), TypeParam(6));
  EXPECT_EQ(range2.front(), TypeParam(1));
  EXPECT_EQ(range1.back(), TypeParam(10));
  EXPECT_EQ(range2.back(), TypeParam(5));
}

TYPED_TEST(contiguous_range, remove_prefix) {
  TypeParam buffer[5] = { TypeParam(1), TypeParam(2),
                          TypeParam(3), TypeParam(4),
                          TypeParam(5) };
  wn::containers::contiguous_range<TypeParam> range(buffer);

  ASSERT_FALSE(range.empty());
  EXPECT_EQ(range.front(), TypeParam(1));
  EXPECT_EQ(range.back(), TypeParam(5));

  range.remove_prefix(2);

  EXPECT_EQ(range.front(), TypeParam(3));
  EXPECT_EQ(range.back(), TypeParam(5));

  EXPECT_DEATH({
      wn::containers::contiguous_range<TypeParam> fail_range;

      fail_range.remove_prefix(0);
    },
    "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid contiguous range"
  );

  EXPECT_DEATH({
      wn::containers::contiguous_range<TypeParam> fail_range(buffer, buffer + 5);

      fail_range.remove_prefix(6);
    },
    "assertion failed!\n\nfile: .*\nline: .*\nmessage: count too large"
  );
}

TYPED_TEST(contiguous_range, remove_suffix) {
  TypeParam buffer[5] = { TypeParam(1), TypeParam(2),
                          TypeParam(3), TypeParam(4),
                          TypeParam(5) };
  wn::containers::contiguous_range<TypeParam> range(buffer);

  ASSERT_FALSE(range.empty());
  EXPECT_EQ(range.front(), TypeParam(1));
  EXPECT_EQ(range.back(), TypeParam(5));

  range.remove_suffix(2);

  EXPECT_EQ(range.front(), TypeParam(1));
  EXPECT_EQ(range.back(), TypeParam(3));

  EXPECT_DEATH({
      wn::containers::contiguous_range<TypeParam> fail_range;

      fail_range.remove_suffix(0);
    },
    "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid contiguous range"
  );

  EXPECT_DEATH({
      wn::containers::contiguous_range<TypeParam> fail_range(buffer, buffer + 5);

      fail_range.remove_suffix(6);
    },
    "assertion failed!\n\nfile: .*\nline: .*\nmessage: count too large"
  );
}

TYPED_TEST(contiguous_range, multiple_ranges_same_source) {
  TypeParam buffer[10] = { TypeParam(1), TypeParam(2),
                           TypeParam(3), TypeParam(4),
                           TypeParam(5), TypeParam(6),
                           TypeParam(7), TypeParam(8),
                           TypeParam(9), TypeParam(10) };
  wn::containers::contiguous_range<TypeParam> range1(buffer);
  wn::containers::contiguous_range<TypeParam> range2(buffer, 3);
  wn::containers::contiguous_range<TypeParam> range3(buffer + 2, 4);

  EXPECT_EQ(range1.size(), 10);
  EXPECT_EQ(range2.size(), 3);
  EXPECT_EQ(range3.size(), 4);

  wn_uint8 count = 1;

  for (auto i = range1.cbegin(); i != range1.cend(); ++i) {
    EXPECT_EQ(*i, TypeParam(count));

    count++;
  }

  count = 1;

  for (auto i = range2.cbegin(); i != range2.cend(); ++i) {
    EXPECT_EQ(*i, TypeParam(count));

    count++;
  }

  count = 3;

  for (auto i = range3.cbegin(); i != range3.cend(); ++i) {
    EXPECT_EQ(*i, TypeParam(count));

    count++;
  }
}