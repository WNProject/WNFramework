// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

template <typename T>
struct intrusive_ptr : testing::Test {};

typedef ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>
    intrusive_ptr_testing_types;

TYPED_TEST_CASE(intrusive_ptr, intrusive_ptr_testing_types);

template <typename T>
struct dummy_intrusive : wn::memory::intrusive_ptr_base {
  dummy_intrusive(const T _value = 0) : m_test(_value) {}

  T test() const {
    return (m_test);
  }

  T m_test;
};

template <typename T>
struct dummy_intrusive_child : dummy_intrusive<T> {};

TYPED_TEST(intrusive_ptr, construction) {
  wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1;

  EXPECT_EQ(pointer1, nullptr);

  wn::testing::allocator allocator;

  {
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(
        wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(
            &allocator, static_cast<TypeParam>(1)));

    EXPECT_NE(pointer2, nullptr);
    ASSERT_NE(pointer2.get(), nullptr);
    EXPECT_EQ(pointer2->test(), static_cast<TypeParam>(1));

    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer3(
        wn::memory::make_intrusive<dummy_intrusive_child<TypeParam>>(
            &allocator));

    EXPECT_NE(pointer3, nullptr);
    ASSERT_NE(pointer3.get(), nullptr);
    EXPECT_EQ(pointer3->test(), static_cast<TypeParam>(0));

    dummy_intrusive<TypeParam>* dummy =
        allocator.construct<dummy_intrusive<TypeParam>>(
            static_cast<TypeParam>(2));
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer4(
        &allocator, dummy);

    EXPECT_NE(pointer4, nullptr);
    ASSERT_NE(pointer4.get(), nullptr);
    EXPECT_EQ(pointer4->test(), static_cast<TypeParam>(2));

    dummy_intrusive_child<TypeParam>* dummy_child =
        allocator.construct<dummy_intrusive_child<TypeParam>>();
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer5(
        &allocator, dummy_child);

    EXPECT_NE(pointer5, nullptr);
    ASSERT_NE(pointer5.get(), nullptr);
    EXPECT_EQ(pointer5->test(), static_cast<TypeParam>(0));
  }
}

TYPED_TEST(intrusive_ptr, use_count) {
  wn::testing::allocator allocator;

  {
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1(
        wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(&allocator));

    EXPECT_NE(pointer1, nullptr);
    EXPECT_EQ(pointer1.use_count(), 1u);
    EXPECT_NE(pointer1.get(), nullptr);

    {
      wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(pointer1);

      EXPECT_NE(pointer1, nullptr);
      EXPECT_NE(pointer2, nullptr);
      EXPECT_EQ(pointer1.use_count(), 2u);
      EXPECT_EQ(pointer2.use_count(), 2u);
      EXPECT_NE(pointer1.get(), nullptr);
      EXPECT_NE(pointer2.get(), nullptr);
    }

    EXPECT_NE(pointer1, nullptr);
    EXPECT_EQ(pointer1.use_count(), 1u);
    EXPECT_NE(pointer1.get(), nullptr);
  }
}

TYPED_TEST(intrusive_ptr, unique) {
  wn::testing::allocator allocator;

  {
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1(
        wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(&allocator));

    EXPECT_NE(pointer1, nullptr);
    EXPECT_TRUE(pointer1.unique());
    EXPECT_NE(pointer1.get(), nullptr);

    {
      wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(pointer1);

      EXPECT_NE(pointer1, nullptr);
      EXPECT_NE(pointer2, nullptr);
      EXPECT_FALSE(pointer1.unique());
      EXPECT_FALSE(pointer2.unique());
      EXPECT_NE(pointer1.get(), nullptr);
      EXPECT_NE(pointer2.get(), nullptr);
    }

    EXPECT_NE(pointer1, nullptr);
    EXPECT_TRUE(pointer1.unique());
    EXPECT_NE(pointer1.get(), nullptr);
  }
}

TYPED_TEST(intrusive_ptr, copying) {
  wn::testing::allocator allocator;

  {
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1(
        wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(&allocator));

    EXPECT_NE(pointer1, nullptr);

    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(pointer1);

    EXPECT_NE(pointer1, nullptr);
    EXPECT_NE(pointer2, nullptr);
    EXPECT_EQ(pointer1.use_count(), 2u);
    EXPECT_EQ(pointer2.use_count(), 2u);
    EXPECT_FALSE(pointer1.unique());
    EXPECT_FALSE(pointer2.unique());
    EXPECT_EQ(pointer1, pointer2);

    {
      wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer3;

      EXPECT_EQ(pointer3, nullptr);

      pointer3 = pointer2;

      EXPECT_NE(pointer1, nullptr);
      EXPECT_NE(pointer2, nullptr);
      EXPECT_NE(pointer3, nullptr);
      EXPECT_EQ(pointer1.use_count(), 3u);
      EXPECT_EQ(pointer2.use_count(), 3u);
      EXPECT_EQ(pointer3.use_count(), 3u);
      EXPECT_FALSE(pointer1.unique());
      EXPECT_FALSE(pointer2.unique());
      EXPECT_FALSE(pointer3.unique());
      EXPECT_EQ(pointer1, pointer2);
      EXPECT_EQ(pointer2, pointer3);
      EXPECT_EQ(pointer1, pointer3);
    }

    EXPECT_NE(pointer1, nullptr);
    EXPECT_NE(pointer2, nullptr);
    EXPECT_EQ(pointer1.use_count(), 2u);
    EXPECT_EQ(pointer2.use_count(), 2u);
    EXPECT_FALSE(pointer1.unique());
    EXPECT_FALSE(pointer2.unique());
    EXPECT_EQ(pointer1, pointer2);
  }
}

TYPED_TEST(intrusive_ptr, moving) {
  wn::testing::allocator allocator;

  {
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1(
        wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(&allocator));

    EXPECT_NE(pointer1, nullptr);

    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(
        std::move(pointer1));

    EXPECT_EQ(pointer1, nullptr);
    EXPECT_NE(pointer2, nullptr);
    EXPECT_EQ(pointer1.use_count(), 0u);
    EXPECT_EQ(pointer2.use_count(), 1u);
    EXPECT_FALSE(pointer1.unique());
    EXPECT_TRUE(pointer2.unique());

    {
      wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer3;

      EXPECT_EQ(pointer3, nullptr);

      pointer3 = std::move(pointer2);

      EXPECT_EQ(pointer1, nullptr);
      EXPECT_EQ(pointer2, nullptr);
      EXPECT_NE(pointer3, nullptr);
      EXPECT_EQ(pointer1.use_count(), 0u);
      EXPECT_EQ(pointer2.use_count(), 0u);
      EXPECT_EQ(pointer3.use_count(), 1u);
      EXPECT_FALSE(pointer1.unique());
      EXPECT_FALSE(pointer2.unique());
      EXPECT_TRUE(pointer3.unique());
    }

    EXPECT_EQ(pointer1, nullptr);
    EXPECT_EQ(pointer2, nullptr);
    EXPECT_EQ(pointer1.use_count(), 0u);
    EXPECT_EQ(pointer2.use_count(), 0u);
    EXPECT_FALSE(pointer1.unique());
    EXPECT_FALSE(pointer2.unique());
  }
}

TYPED_TEST(intrusive_ptr, swap) {
  wn::testing::allocator allocator;

  {
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1(
        wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(&allocator));
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(
        wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(&allocator));

    EXPECT_NE(pointer1, nullptr);
    EXPECT_NE(pointer2, nullptr);
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
}

TYPED_TEST(intrusive_ptr, reset) {
  wn::testing::allocator allocator;

  {
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer1(
        wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(&allocator));
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer2(
        wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(&allocator));
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer3(
        wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(&allocator));
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer4(
        wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(&allocator));

    EXPECT_NE(pointer1, nullptr);
    EXPECT_NE(pointer2, nullptr);
    EXPECT_NE(pointer3, nullptr);
    EXPECT_NE(pointer4, nullptr);

    pointer1.reset();
    pointer2.reset(nullptr);

    dummy_intrusive<TypeParam>* dummy1 = nullptr;

    pointer3.reset(&allocator, dummy1);

    dummy_intrusive_child<TypeParam>* dummy2 = nullptr;

    pointer4.reset(&allocator, dummy2);

    EXPECT_EQ(pointer1, nullptr);
    EXPECT_EQ(pointer1.use_count(), 0u);
    EXPECT_EQ(pointer2, nullptr);
    EXPECT_EQ(pointer2.use_count(), 0u);
    EXPECT_EQ(pointer3, nullptr);
    EXPECT_EQ(pointer3.use_count(), 0u);
    EXPECT_EQ(pointer4, nullptr);
    EXPECT_EQ(pointer4.use_count(), 0u);
  }
}

TYPED_TEST(intrusive_ptr, release) {
  wn::testing::allocator allocator;

  {
    wn::memory::intrusive_ptr<dummy_intrusive<TypeParam>> pointer(
        wn::memory::make_intrusive<dummy_intrusive<TypeParam>>(
            &allocator, static_cast<TypeParam>(1)));

    ASSERT_NE(pointer, nullptr);
    EXPECT_EQ(pointer->test(), static_cast<TypeParam>(1));

    const dummy_intrusive<TypeParam>* pointer_raw = pointer.get();
    dummy_intrusive<TypeParam>* value = pointer.release();

    ASSERT_NE(value, nullptr);
    EXPECT_EQ(value->test(), static_cast<TypeParam>(1));
    EXPECT_EQ(value->reference_count(), 1u);
    EXPECT_EQ(value, pointer_raw);
    EXPECT_EQ(pointer, nullptr);
    EXPECT_EQ(pointer.use_count(), 0u);

    allocator.destroy(value);
  }
}