// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNContiguousRange.h"
#include "executable_test/inc/WNTestHarness.h"

template <typename _Type>
struct contiguous_range : ::testing::Test {};

// 72 bit struct
struct dummy1 {
  inline dummy1(const uint8_t _value) : m_value1(_value), m_value2(_value) {}

  inline void operator=(const dummy1& _other) volatile {
    m_value1 = _other.m_value1;
    m_value2 = _other.m_value2;
  }

  uint8_t m_value1;
  uint64_t m_value2;
};

// non trivial assignment
struct dummy2 {
  inline dummy2(const uint8_t _value) : m_value(_value) {}

  inline dummy2& operator=(const dummy2& _other) {
    m_value = _other.m_value + 1;

    return *this;
  }

  uint8_t m_value;
};

inline bool operator==(const dummy1& _lhs, const dummy1& _rhs) {
  return (_lhs.m_value1 == _rhs.m_value1 && _lhs.m_value2 == _rhs.m_value2);
}

inline bool operator==(const dummy2& _lhs, const dummy2& _rhs) {
  return (_lhs.m_value == _rhs.m_value);
}

typedef ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t, dummy1>
    contiguous_range_testing_types;

TYPED_TEST_SUITE(contiguous_range, contiguous_range_testing_types);

TYPED_TEST(contiguous_range, construction) {
  TypeParam buffer[5] = {
      TypeParam(1), TypeParam(2), TypeParam(3), TypeParam(4), TypeParam(5)};
  wn::containers::contiguous_range<TypeParam> range1;
  wn::containers::contiguous_range<TypeParam> range2(nullptr);
  wn::containers::contiguous_range<TypeParam> range4(buffer, buffer + 5);
  wn::containers::contiguous_range<const TypeParam> range5(buffer, 5);
  wn::containers::contiguous_range<TypeParam> range6(buffer);
  wn::containers::contiguous_range<TypeParam> range7(range6);

  EXPECT_TRUE(range1.empty());
  EXPECT_EQ(range1.data(), nullptr);
  EXPECT_TRUE(range2.empty());
  EXPECT_EQ(range2.data(), nullptr);
  EXPECT_FALSE(range4.empty());
  EXPECT_EQ(range4.data(), buffer);
  EXPECT_FALSE(range5.empty());
  EXPECT_EQ(range5.data(), buffer);
  EXPECT_FALSE(range6.empty());
  EXPECT_EQ(range6.data(), buffer);
  EXPECT_FALSE(range7.empty());
  EXPECT_EQ(range7.data(), buffer);

  wn::containers::contiguous_range<const TypeParam> range8(
      wn::core::move(range7));

  EXPECT_TRUE(range7.empty());
  EXPECT_EQ(range7.data(), nullptr);
  EXPECT_FALSE(range8.empty());
  EXPECT_EQ(range8.data(), buffer);

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      {
        TypeParam* null = nullptr;
        const wn::containers::contiguous_range<TypeParam> range(
            null, buffer + 5);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid input "
      "parameters, both must be null or non-null");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      {
        TypeParam* null = nullptr;
        const wn::containers::contiguous_range<TypeParam> range(buffer, null);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid input "
      "parameters, both must be null or non-null");
}

TYPED_TEST(contiguous_range, assignment) {
  TypeParam buffer[5] = {
      TypeParam(1), TypeParam(2), TypeParam(3), TypeParam(4), TypeParam(5)};
  wn::containers::contiguous_range<TypeParam> range1(buffer);

  range1 = nullptr;

  wn::containers::contiguous_range<TypeParam> range2;

  range2 = buffer;

  wn::containers::contiguous_range<TypeParam> range3;

  range3 = range2;

  EXPECT_TRUE(range1.empty());
  EXPECT_EQ(range1.data(), nullptr);
  EXPECT_FALSE(range2.empty());
  EXPECT_EQ(range2.data(), buffer);
  EXPECT_FALSE(range3.empty());
  EXPECT_EQ(range3.data(), buffer);

  wn::containers::contiguous_range<const TypeParam> range4;

  range4 = wn::core::move(range3);

  EXPECT_TRUE(range3.empty());
  EXPECT_EQ(range3.data(), nullptr);
  EXPECT_FALSE(range4.empty());
  EXPECT_EQ(range4.data(), buffer);
}

TYPED_TEST(contiguous_range, capacity) {
  TypeParam buffer[5] = {
      TypeParam(1), TypeParam(2), TypeParam(3), TypeParam(4), TypeParam(5)};
  const wn::containers::contiguous_range<TypeParam> range1(buffer);

  EXPECT_EQ(range1.size(), 5u);
  EXPECT_EQ(range1.max_size(), 5u);
  EXPECT_EQ(range1.max_size(), range1.size());

  const wn::containers::contiguous_range<TypeParam> range2(buffer + 1, 3);

  EXPECT_EQ(range2.size(), 3u);
  EXPECT_EQ(range2.max_size(), 3u);
  EXPECT_EQ(range2.max_size(), range2.size());
}

TYPED_TEST(contiguous_range, access) {
  TypeParam buffer[5] = {
      TypeParam(1), TypeParam(2), TypeParam(3), TypeParam(4), TypeParam(5)};
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

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      {
        const wn::containers::contiguous_range<TypeParam> range;

        range.at(0);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid contiguous "
      "range");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      {
        const wn::containers::contiguous_range<TypeParam> range(
            buffer, buffer + 5);

        range.at(5);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: index out of bounds");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      {
        const wn::containers::contiguous_range<TypeParam> range;

        range[0];
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid contiguous "
      "range");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      {
        const wn::containers::contiguous_range<TypeParam> range(
            buffer, buffer + 5);

        range[5];
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: index out of bounds");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      {
        const wn::containers::contiguous_range<TypeParam> range;

        range.front();
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid contiguous "
      "range");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      {
        const wn::containers::contiguous_range<TypeParam> range;

        range.back();
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid contiguous "
      "range");
}

TYPED_TEST(contiguous_range, iteration) {
  TypeParam buffer[5] = {
      TypeParam(1), TypeParam(2), TypeParam(3), TypeParam(4), TypeParam(5)};
  wn::containers::contiguous_range<TypeParam> range1(buffer);

  uint8_t count = 1;

  for (auto i = range1.begin(); i != range1.end(); ++i) {
    EXPECT_EQ(*i, TypeParam(count));

    count++;
  }

  count = 1;

  // check constness
  for (typename wn::containers::contiguous_range<TypeParam>::const_iterator i =
           range1.begin();
       i != range1.end(); ++i) {
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

  // check constness
  for (typename wn::containers::contiguous_range<
           TypeParam>::const_reverse_iterator i = range1.rbegin();
       i != range1.rend(); ++i) {
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

TYPED_TEST(contiguous_range, clear) {
  TypeParam buffer[5] = {
      TypeParam(1), TypeParam(2), TypeParam(3), TypeParam(4), TypeParam(5)};
  wn::containers::contiguous_range<TypeParam> range(buffer);

  EXPECT_FALSE(range.empty());
  EXPECT_EQ(range.size(), 5u);

  range.clear();

  EXPECT_TRUE(range.empty());
  EXPECT_EQ(range.size(), 0u);
}

TYPED_TEST(contiguous_range, swap) {
  TypeParam buffer1[5] = {
      TypeParam(1), TypeParam(2), TypeParam(3), TypeParam(4), TypeParam(5)};
  TypeParam buffer2[5] = {
      TypeParam(6), TypeParam(7), TypeParam(8), TypeParam(9), TypeParam(10)};
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
  TypeParam buffer[5] = {
      TypeParam(1), TypeParam(2), TypeParam(3), TypeParam(4), TypeParam(5)};
  wn::containers::contiguous_range<TypeParam> range(buffer);

  ASSERT_FALSE(range.empty());
  EXPECT_EQ(range.front(), TypeParam(1));
  EXPECT_EQ(range.back(), TypeParam(5));

  range.remove_prefix(2);

  EXPECT_EQ(range.front(), TypeParam(3));
  EXPECT_EQ(range.back(), TypeParam(5));

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      {
        wn::containers::contiguous_range<TypeParam> fail_range;

        fail_range.remove_prefix(0);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid contiguous "
      "range");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      {
        wn::containers::contiguous_range<TypeParam> fail_range(
            buffer, buffer + 5);

        fail_range.remove_prefix(6);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: count too large");
}

TYPED_TEST(contiguous_range, remove_suffix) {
  TypeParam buffer[5] = {
      TypeParam(1), TypeParam(2), TypeParam(3), TypeParam(4), TypeParam(5)};
  wn::containers::contiguous_range<TypeParam> range(buffer);

  ASSERT_FALSE(range.empty());
  EXPECT_EQ(range.front(), TypeParam(1));
  EXPECT_EQ(range.back(), TypeParam(5));

  range.remove_suffix(2);

  EXPECT_EQ(range.front(), TypeParam(1));
  EXPECT_EQ(range.back(), TypeParam(3));

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      {
        wn::containers::contiguous_range<TypeParam> fail_range;

        fail_range.remove_suffix(0);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: invalid contiguous "
      "range");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      {
        wn::containers::contiguous_range<TypeParam> fail_range(
            buffer, buffer + 5);

        fail_range.remove_suffix(6);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: count too large");
}

TYPED_TEST(contiguous_range, multiple_ranges_same_source) {
  TypeParam buffer[10] = {TypeParam(1), TypeParam(2), TypeParam(3),
      TypeParam(4), TypeParam(5), TypeParam(6), TypeParam(7), TypeParam(8),
      TypeParam(9), TypeParam(10)};
  wn::containers::contiguous_range<TypeParam> range1(buffer);
  wn::containers::contiguous_range<TypeParam> range2(buffer, 3);
  wn::containers::contiguous_range<TypeParam> range3(buffer + 2, 4);

  EXPECT_EQ(range1.size(), 10u);
  EXPECT_EQ(range2.size(), 3u);
  EXPECT_EQ(range3.size(), 4u);

  uint8_t count = 1;

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

TYPED_TEST(contiguous_range, const_range) {
  TypeParam buffer[10] = {TypeParam(1), TypeParam(2), TypeParam(3),
      TypeParam(4), TypeParam(5), TypeParam(6), TypeParam(7), TypeParam(8),
      TypeParam(9), TypeParam(10)};

  using ConstTypeParam = typename wn::core::add_const<TypeParam>::type;

  wn::containers::contiguous_range<ConstTypeParam> range(buffer, 10);

  for (auto i = 0u; i < 10u; ++i) {
    EXPECT_EQ(buffer[i], range[i]);
  }
}

TYPED_TEST(contiguous_range, read_only_range) {
  TypeParam buffer[10] = {TypeParam(1), TypeParam(2), TypeParam(3),
      TypeParam(4), TypeParam(5), TypeParam(6), TypeParam(7), TypeParam(8),
      TypeParam(9), TypeParam(10)};
  wn::containers::read_only_contiguous_range<TypeParam> range(buffer, 10);

  for (auto i = 0u; i < 10u; ++i) {
    EXPECT_EQ(buffer[i], range[i]);
  }
}

TYPED_TEST(contiguous_range, write_only_range) {
  TypeParam buffer1[10] = {TypeParam(1), TypeParam(2), TypeParam(3),
      TypeParam(4), TypeParam(5), TypeParam(6), TypeParam(7), TypeParam(8),
      TypeParam(9), TypeParam(10)};
  wn::containers::write_only_contiguous_range<TypeParam> range1(buffer1, 10);

  for (auto& value : range1) {
    value = TypeParam(11);
  }

  for (auto& value : buffer1) {
    EXPECT_EQ(value, TypeParam(11));
  }

  TypeParam buffer2[10] = {TypeParam(12), TypeParam(12), TypeParam(12),
      TypeParam(12), TypeParam(12), TypeParam(12), TypeParam(12), TypeParam(12),
      TypeParam(12), TypeParam(12)};

  wn::containers::copy_to(&range1, buffer2, 10);

  for (auto& value : buffer2) {
    EXPECT_EQ(value, TypeParam(12));
  }

  TypeParam buffer3[10] = {TypeParam(13), TypeParam(13), TypeParam(13),
      TypeParam(13), TypeParam(13), TypeParam(13), TypeParam(13), TypeParam(13),
      TypeParam(13), TypeParam(13)};

  wn::containers::contiguous_range<TypeParam> range2(buffer3, 10);

  wn::containers::copy_to(&range1, range2);

  for (auto& value : buffer3) {
    EXPECT_EQ(value, TypeParam(13));
  }
}

TYPED_TEST(contiguous_range, copy_write_only_range) {
  TypeParam buffer1[10] = {TypeParam(1), TypeParam(2), TypeParam(3),
      TypeParam(4), TypeParam(5), TypeParam(6), TypeParam(7), TypeParam(8),
      TypeParam(9), TypeParam(10)};
  wn::containers::write_only_contiguous_range<TypeParam> range1(buffer1, 10);

  wn::containers::write_only_contiguous_range<TypeParam> range2(range1);

  for (auto& value : range2) {
    value = TypeParam(11);
  }

  for (auto& value : buffer1) {
    EXPECT_EQ(value, TypeParam(11));
  }
}

TEST(contiguous_range, copy_to_trivial_vs_non_trivial) {
  uint8_t buffer1[10] = {uint8_t(1), uint8_t(1), uint8_t(1), uint8_t(1),
      uint8_t(1), uint8_t(1), uint8_t(1), uint8_t(1), uint8_t(1), uint8_t(1)};

  const wn::containers::contiguous_range<uint8_t> range1(buffer1);

  uint8_t buffer2[10] = {0};

  wn::containers::contiguous_range<uint8_t> range2(buffer2);

  wn::containers::copy_to(&range2, range1);

  for (auto& value : buffer1) {
    EXPECT_EQ(value, uint8_t(1));
  }

  dummy2 buffer3[10] = {dummy2(1), dummy2(1), dummy2(1), dummy2(1), dummy2(1),
      dummy2(1), dummy2(1), dummy2(1), dummy2(1), dummy2(1)};

  wn::containers::contiguous_range<const dummy2> range3(buffer3);

  dummy2 buffer4[10] = {dummy2(0), dummy2(0), dummy2(0), dummy2(0), dummy2(0),
      dummy2(0), dummy2(0), dummy2(0), dummy2(0), dummy2(0)};

  wn::containers::contiguous_range<dummy2> range4(buffer4);

  wn::containers::copy_to(&range4, range3);

  for (auto& value : buffer4) {
    EXPECT_EQ(value, dummy2(2));
  }
}

TYPED_TEST(contiguous_range, is_read_only_contiguous_range) {
  EXPECT_TRUE(wn::containers::is_read_only_contiguous_range<
      wn::containers::read_only_contiguous_range<TypeParam>>::value);
  EXPECT_TRUE(wn::containers::is_read_only_contiguous_range<
      wn::containers::contiguous_range<const TypeParam>>::value);
  EXPECT_FALSE(wn::containers::is_read_only_contiguous_range<
      wn::containers::write_only_contiguous_range<TypeParam>>::value);
  EXPECT_FALSE(wn::containers::is_read_only_contiguous_range<wn::containers::
          contiguous_range<wn::containers::write_only<TypeParam>>>::value);
}

TYPED_TEST(contiguous_range, is_write_only_contiguous_range) {
  EXPECT_TRUE(wn::containers::is_write_only_contiguous_range<
      wn::containers::write_only_contiguous_range<TypeParam>>::value);
  EXPECT_TRUE(wn::containers::is_write_only_contiguous_range<wn::containers::
          contiguous_range<wn::containers::write_only<TypeParam>>>::value);
  EXPECT_FALSE(wn::containers::is_write_only_contiguous_range<
      wn::containers::read_only_contiguous_range<TypeParam>>::value);
  EXPECT_FALSE(wn::containers::is_write_only_contiguous_range<
      wn::containers::contiguous_range<const TypeParam>>::value);
}
