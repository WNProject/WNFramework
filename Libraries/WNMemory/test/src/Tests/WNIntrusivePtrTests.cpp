// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMemory/test/inc/Common.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

template <typename T>
struct intrusive_ptr : testing::Test {};

typedef ::testing::Types<wn_uint8, wn_uint16, wn_uint32, wn_uint64>
  intrusive_ptr_testing_types;

TYPED_TEST_CASE(intrusive_ptr, intrusive_ptr_testing_types);

template <typename T>
struct dummy_intrusive : wn::memory::intrusive_ptr_base {
  dummy_intrusive(const T _value = 0) :
    m_test(_value) {
  }

  T test() const {
    return(m_test);
  }

  T m_test;
};

template <typename T>
struct dummy_intrusive_child : dummy_intrusive<T> {};

TYPED_TEST(intrusive_ptr, construction) {
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1;

  EXPECT_EQ(pointer1, wn_nullptr);

  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(
    wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(
      static_cast<TypeParam>(1)));

  EXPECT_NE(pointer2, wn_nullptr);
  ASSERT_NE(pointer2.get(), wn_nullptr);
  EXPECT_EQ(pointer2->test(), static_cast<TypeParam>(1));

  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer3(
    wn::memory::make_intrusive<dummy_intrusive_child<TypeParam>>());

  EXPECT_NE(pointer3, wn_nullptr);
  ASSERT_NE(pointer3.get(), wn_nullptr);
  EXPECT_EQ(pointer3->test(), 0);

  dummy_intrusive<TypeParam>* dummy =
    wn::memory::construct<dummy_intrusive<TypeParam>>(
      static_cast<TypeParam>(2));
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer4(dummy);

  EXPECT_NE(pointer4, wn_nullptr);
  ASSERT_NE(pointer4.get(), wn_nullptr);
  EXPECT_EQ(pointer4->test(), static_cast<TypeParam>(2));

  dummy_intrusive_child<TypeParam>* dummy_child =
    wn::memory::construct<dummy_intrusive_child<TypeParam>>();
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer5(dummy_child);

  EXPECT_NE(pointer5, wn_nullptr);
  ASSERT_NE(pointer5.get(), wn_nullptr);
  EXPECT_EQ(pointer5->test(), 0);

  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer6(
    pointer5.get(), wn_false);

  EXPECT_NE(pointer6, wn_nullptr);
  EXPECT_NE(pointer6.get(), wn_nullptr);
  EXPECT_EQ(pointer6.use_count(), 1);

  dummy_child->add_reference();

  EXPECT_EQ(pointer6.use_count(), 2);
}

TYPED_TEST(intrusive_ptr, use_count) {
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1(
    wn::memory::make_intrusive<dummy_intrusive<TypeParam>>());

  EXPECT_NE(pointer1, wn_nullptr);
  EXPECT_EQ(pointer1.use_count(), 1);
  EXPECT_NE(pointer1.get(), wn_nullptr);

  {
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(pointer1);

    EXPECT_NE(pointer1, wn_nullptr);
    EXPECT_NE(pointer2, wn_nullptr);
    EXPECT_EQ(pointer1.use_count(), 2);
    EXPECT_EQ(pointer2.use_count(), 2);
    EXPECT_NE(pointer1.get(), wn_nullptr);
    EXPECT_NE(pointer2.get(), wn_nullptr);
  }

  EXPECT_NE(pointer1, wn_nullptr);
  EXPECT_EQ(pointer1.use_count(), 1);
  EXPECT_NE(pointer1.get(), wn_nullptr);
}

TYPED_TEST(intrusive_ptr, unique) {
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1(
    wn::memory::make_intrusive<dummy_intrusive<TypeParam>>());

  EXPECT_NE(pointer1, wn_nullptr);
  EXPECT_TRUE(pointer1.unique());
  EXPECT_NE(pointer1.get(), wn_nullptr);

  {
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(pointer1);

    EXPECT_NE(pointer1, wn_nullptr);
    EXPECT_NE(pointer2, wn_nullptr);
    EXPECT_FALSE(pointer1.unique());
    EXPECT_FALSE(pointer2.unique());
    EXPECT_NE(pointer1.get(), wn_nullptr);
    EXPECT_NE(pointer2.get(), wn_nullptr);
  }

  EXPECT_NE(pointer1, wn_nullptr);
  EXPECT_TRUE(pointer1.unique());
  EXPECT_NE(pointer1.get(), wn_nullptr);
}

TYPED_TEST(intrusive_ptr, copying) {
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1(
    wn::memory::make_intrusive<dummy_intrusive<TypeParam>>());

  EXPECT_NE(pointer1, wn_nullptr);

  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(pointer1);

  EXPECT_NE(pointer1, wn_nullptr);
  EXPECT_NE(pointer2, wn_nullptr);
  EXPECT_EQ(pointer1.use_count(), 2);
  EXPECT_EQ(pointer2.use_count(), 2);
  EXPECT_FALSE(pointer1.unique());
  EXPECT_FALSE(pointer2.unique());
  EXPECT_EQ(pointer1, pointer2);

  {
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer3;

    EXPECT_EQ(pointer3, wn_nullptr);

    pointer3 = pointer2;

    EXPECT_NE(pointer1, wn_nullptr);
    EXPECT_NE(pointer2, wn_nullptr);
    EXPECT_NE(pointer3, wn_nullptr);
    EXPECT_EQ(pointer1.use_count(), 3);
    EXPECT_EQ(pointer2.use_count(), 3);
    EXPECT_EQ(pointer3.use_count(), 3);
    EXPECT_FALSE(pointer1.unique());
    EXPECT_FALSE(pointer2.unique());
    EXPECT_FALSE(pointer3.unique());
    EXPECT_EQ(pointer1, pointer2);
    EXPECT_EQ(pointer2, pointer3);
    EXPECT_EQ(pointer1, pointer3);
  }

  EXPECT_NE(pointer1, wn_nullptr);
  EXPECT_NE(pointer2, wn_nullptr);
  EXPECT_EQ(pointer1.use_count(), 2);
  EXPECT_EQ(pointer2.use_count(), 2);
  EXPECT_FALSE(pointer1.unique());
  EXPECT_FALSE(pointer2.unique());
  EXPECT_EQ(pointer1, pointer2);
}

TYPED_TEST(intrusive_ptr, moving) {
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1(
    wn::memory::make_intrusive<dummy_intrusive<TypeParam>>());

  EXPECT_NE(pointer1, wn_nullptr);

  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(
    std::move(pointer1));

  EXPECT_EQ(pointer1, wn_nullptr);
  EXPECT_NE(pointer2, wn_nullptr);
  EXPECT_EQ(pointer1.use_count(), 0);
  EXPECT_EQ(pointer2.use_count(), 1);
  EXPECT_FALSE(pointer1.unique());
  EXPECT_TRUE(pointer2.unique());

  {
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer3;

    EXPECT_EQ(pointer3, wn_nullptr);

    pointer3 = std::move(pointer2);

    EXPECT_EQ(pointer1, wn_nullptr);
    EXPECT_EQ(pointer2, wn_nullptr);
    EXPECT_NE(pointer3, wn_nullptr);
    EXPECT_EQ(pointer1.use_count(), 0);
    EXPECT_EQ(pointer2.use_count(), 0);
    EXPECT_EQ(pointer3.use_count(), 1);
    EXPECT_FALSE(pointer1.unique());
    EXPECT_FALSE(pointer2.unique());
    EXPECT_TRUE(pointer3.unique());
  }

  EXPECT_EQ(pointer1, wn_nullptr);
  EXPECT_EQ(pointer2, wn_nullptr);
  EXPECT_EQ(pointer1.use_count(), 0);
  EXPECT_EQ(pointer2.use_count(), 0);
  EXPECT_FALSE(pointer1.unique());
  EXPECT_FALSE(pointer2.unique());
}

TYPED_TEST(intrusive_ptr, swap) {
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1(
    wn::memory::make_intrusive<dummy_intrusive<TypeParam>>());
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(
    wn::memory::make_intrusive<dummy_intrusive<TypeParam>>());

  EXPECT_NE(pointer1, wn_nullptr);
  EXPECT_NE(pointer2, wn_nullptr);
  EXPECT_TRUE(pointer1.unique());
  EXPECT_TRUE(pointer2.unique());

  const dummy_intrusive<TypeParam>* pointer1_raw = pointer1.get();
  const dummy_intrusive<TypeParam>* pointer2_raw = pointer2.get();

  pointer1.swap(pointer2);

  EXPECT_EQ(pointer1_raw, pointer2.get());
  EXPECT_EQ(pointer2_raw, pointer1.get());
  EXPECT_TRUE(pointer1.unique());
  EXPECT_TRUE(pointer2.unique());

  std::swap(pointer1, pointer2);

  EXPECT_EQ(pointer1_raw, pointer1.get());
  EXPECT_EQ(pointer2_raw, pointer2.get());
  EXPECT_TRUE(pointer1.unique());
  EXPECT_TRUE(pointer2.unique());
}

TYPED_TEST(intrusive_ptr, reset) {
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1(
    wn::memory::make_intrusive<dummy_intrusive<TypeParam>>());
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(
    wn::memory::make_intrusive<dummy_intrusive<TypeParam>>());
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer3(
    wn::memory::make_intrusive<dummy_intrusive<TypeParam>>());
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer4(
    wn::memory::make_intrusive<dummy_intrusive<TypeParam>>());

  EXPECT_NE(pointer1, wn_nullptr);
  EXPECT_NE(pointer2, wn_nullptr);
  EXPECT_NE(pointer3, wn_nullptr);
  EXPECT_NE(pointer4, wn_nullptr);

  pointer1.reset();
  pointer2.reset(wn_nullptr);

  dummy_intrusive<TypeParam>* dummy1 = wn_nullptr;

  pointer3.reset(dummy1);

  dummy_intrusive_child<TypeParam>* dummy2 = wn_nullptr;

  pointer4.reset(dummy2);

  EXPECT_EQ(pointer1, wn_nullptr);
  EXPECT_EQ(pointer1.use_count(), 0);
  EXPECT_EQ(pointer2, wn_nullptr);
  EXPECT_EQ(pointer2.use_count(), 0);
  EXPECT_EQ(pointer3, wn_nullptr);
  EXPECT_EQ(pointer3.use_count(), 0);
  EXPECT_EQ(pointer4, wn_nullptr);
  EXPECT_EQ(pointer4.use_count(), 0);
}

TYPED_TEST(intrusive_ptr, release) {
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer(
    wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(
      static_cast<TypeParam>(1)));

  ASSERT_NE(pointer, wn_nullptr);
  EXPECT_EQ(pointer->test(), 1);

  const dummy_intrusive<TypeParam>* pointer_raw = pointer.get();
  const dummy_intrusive<TypeParam>* value = pointer.release();

  ASSERT_NE(value, wn_nullptr);
  EXPECT_EQ(value->test(), static_cast<TypeParam>(1));
  EXPECT_EQ(value->reference_count(), 1);
  EXPECT_EQ(value, pointer_raw);
  EXPECT_EQ(pointer, wn_nullptr);
  EXPECT_EQ(pointer.use_count(), 0);

  wn::memory::destroy(value);
}