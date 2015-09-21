// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMemory/test/inc/Common.h"
#include "WNMemory/inc/WNUniquePtr.h"

template <typename T>
struct unique_ptr : testing::Test {};

typedef ::testing::Types<wn_uint8, wn_uint16, wn_uint32, wn_uint64>
  unique_ptr_testing_types;

TYPED_TEST_CASE(unique_ptr, unique_ptr_testing_types);

template <typename T>
class dummy_destroyer {
public:
  dummy_destroyer(wn_uint32& test) :
    m_test(test) {
  }

  dummy_destroyer(const dummy_destroyer& d) :
    m_test(d.m_test) {
  };

  dummy_destroyer& operator = (const dummy_destroyer& d) {
    m_test = d.m_test;
  };

  wn_void operator () (T* ptr) const {
    m_test++;

    wn::memory::destroy(ptr);
  }

private:
  wn_uint32& m_test;
};

TYPED_TEST(unique_ptr, construction) {
  wn::memory::unique_ptr<TypeParam> pointer1;

  EXPECT_EQ(pointer1, wn_nullptr);

  wn::memory::unique_ptr<TypeParam> pointer2(
    wn::memory::make_unique<TypeParam>(static_cast<TypeParam>(1)));

  EXPECT_NE(pointer2, wn_nullptr);
  ASSERT_NE(pointer2.get(), wn_nullptr);
  EXPECT_EQ(*pointer2, static_cast<TypeParam>(1));

  TypeParam* value1 =
    wn::memory::construct<TypeParam>(static_cast<TypeParam>(2));
  wn::memory::unique_ptr<TypeParam> pointer3(value1);

  EXPECT_NE(pointer3, wn_nullptr);
  ASSERT_NE(pointer3.get(), wn_nullptr);
  EXPECT_EQ(*pointer3, 2);

  wn_uint32 num_destroys = 0;

  {
    TypeParam* value2 =
      wn::memory::construct<TypeParam>(static_cast<TypeParam>(3));
    dummy_destroyer<TypeParam> destroyer1(num_destroys);
    wn::memory::unique_ptr<TypeParam, dummy_destroyer<TypeParam>> pointer4(
      value2, destroyer1);

    EXPECT_NE(pointer4, wn_nullptr);
    ASSERT_NE(pointer4.get(), wn_nullptr);
    EXPECT_EQ(*pointer4, 3);

    TypeParam* value3 =
      wn::memory::construct<TypeParam>(static_cast<TypeParam>(4));
    dummy_destroyer<TypeParam> destroyer2(num_destroys);
    wn::memory::unique_ptr<TypeParam, dummy_destroyer<TypeParam>> pointer5(
      value3, std::move(destroyer2));

    EXPECT_NE(pointer5, wn_nullptr);
    ASSERT_NE(pointer5.get(), wn_nullptr);
    EXPECT_EQ(*pointer5, 4);
  }

  EXPECT_EQ(num_destroys, 2);
}

TYPED_TEST(unique_ptr, moving) {
  wn::memory::unique_ptr<TypeParam> pointer1(
    wn::memory::make_unique<TypeParam>(static_cast<TypeParam>(1)));

  EXPECT_NE(pointer1, wn_nullptr);

  wn::memory::unique_ptr<TypeParam> pointer2(std::move(pointer1));

  EXPECT_EQ(pointer1, wn_nullptr);
  ASSERT_NE(pointer2, wn_nullptr);
  EXPECT_EQ(*pointer2, static_cast<TypeParam>(1));

  {
    wn::memory::unique_ptr<TypeParam> pointer3;

    EXPECT_EQ(pointer3, wn_nullptr);

    pointer3 = std::move(pointer2);

    EXPECT_EQ(pointer1, wn_nullptr);
    EXPECT_EQ(pointer2, wn_nullptr);
    ASSERT_NE(pointer3, wn_nullptr);
    EXPECT_EQ(*pointer3, static_cast<TypeParam>(1));
  }

  EXPECT_EQ(pointer1, wn_nullptr);
  EXPECT_EQ(pointer2, wn_nullptr);
}

TYPED_TEST(unique_ptr, swap) {
  wn::memory::unique_ptr<TypeParam> pointer1(
    wn::memory::make_unique<TypeParam>(static_cast<TypeParam>(1)));
  wn::memory::unique_ptr<TypeParam> pointer2(
    wn::memory::make_unique<TypeParam>(static_cast<TypeParam>(2)));

  ASSERT_NE(pointer1, wn_nullptr);
  ASSERT_NE(pointer2, wn_nullptr);
  EXPECT_EQ(*pointer1, static_cast<TypeParam>(1));
  EXPECT_EQ(*pointer2, static_cast<TypeParam>(2));

  const TypeParam* pointer1_raw = pointer1.get();
  const TypeParam* pointer2_raw = pointer2.get();

  pointer1.swap(pointer2);

  ASSERT_NE(pointer1, wn_nullptr);
  ASSERT_NE(pointer2, wn_nullptr);
  EXPECT_EQ(pointer1_raw, pointer2.get());
  EXPECT_EQ(pointer2_raw, pointer1.get());
  EXPECT_EQ(*pointer2, static_cast<TypeParam>(1));
  EXPECT_EQ(*pointer1, static_cast<TypeParam>(2));

  std::swap(pointer1, pointer2);

  ASSERT_NE(pointer1, wn_nullptr);
  ASSERT_NE(pointer2, wn_nullptr);
  EXPECT_EQ(pointer1_raw, pointer1.get());
  EXPECT_EQ(pointer2_raw, pointer2.get());
  EXPECT_EQ(*pointer1, static_cast<TypeParam>(1));
  EXPECT_EQ(*pointer2, static_cast<TypeParam>(2));
}

TYPED_TEST(unique_ptr, reset) {
  wn::memory::unique_ptr<TypeParam> pointer1(
    wn::memory::make_unique<TypeParam>(static_cast<TypeParam>(1)));
  wn::memory::unique_ptr<TypeParam> pointer2(
    wn::memory::make_unique<TypeParam>(static_cast<TypeParam>(2)));
  wn::memory::unique_ptr<TypeParam> pointer3(
    wn::memory::make_unique<TypeParam>(static_cast<TypeParam>(3)));
  wn::memory::unique_ptr<TypeParam> pointer4(
    wn::memory::make_unique<TypeParam>(static_cast<TypeParam>(4)));

  ASSERT_NE(pointer1, wn_nullptr);
  ASSERT_NE(pointer2, wn_nullptr);
  ASSERT_NE(pointer3, wn_nullptr);
  ASSERT_NE(pointer4, wn_nullptr);
  EXPECT_EQ(*pointer1, static_cast<TypeParam>(1));
  EXPECT_EQ(*pointer2, static_cast<TypeParam>(2));
  EXPECT_EQ(*pointer3, static_cast<TypeParam>(3));
  EXPECT_EQ(*pointer4, static_cast<TypeParam>(4));

  pointer1.reset();

  EXPECT_EQ(pointer1, wn_nullptr);

  pointer2.reset(wn_nullptr);

  EXPECT_EQ(pointer2, wn_nullptr);

  TypeParam* value =
    wn::memory::construct<TypeParam>(static_cast<TypeParam>(5));

  pointer3.reset(value);

  ASSERT_NE(pointer3, wn_nullptr);
  EXPECT_EQ(*pointer3, 5);
}

TYPED_TEST(unique_ptr, release) {
  wn::memory::unique_ptr<TypeParam> pointer(
    wn::memory::make_unique<TypeParam>(static_cast<TypeParam>(1)));

  ASSERT_NE(pointer, wn_nullptr);
  EXPECT_EQ(*pointer, static_cast<TypeParam>(1));

  TypeParam* pointer_raw = pointer.get();
  TypeParam* value = pointer.release();

  ASSERT_NE(value, wn_nullptr);
  EXPECT_EQ(*value, static_cast<TypeParam>(1));
  EXPECT_EQ(value, pointer_raw);
  EXPECT_EQ(pointer, wn_nullptr);

  wn::memory::destroy(value);
}
