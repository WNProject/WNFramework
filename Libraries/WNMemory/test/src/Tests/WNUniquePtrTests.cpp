// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNMemory/inc/WNUniquePtr.h"

template <typename T>
struct unique_ptr : testing::Test {};

typedef ::testing::Types<uint8_t, int16_t, uint32_t, int64_t>
    unique_ptr_testing_types;

TYPED_TEST_CASE(unique_ptr, unique_ptr_testing_types);

template <typename T>
class dummy_destroyer {
public:
  WN_FORCE_INLINE dummy_destroyer(uint32_t& test) : m_test(test) {}

  WN_FORCE_INLINE dummy_destroyer(const dummy_destroyer& d)
    : m_test(d.m_test) {}

  WN_FORCE_INLINE dummy_destroyer& operator=(const dummy_destroyer& d) {
    m_test = d.m_test;
  };

  WN_FORCE_INLINE void operator()(T* ptr) const {
    m_test++;

    wn::memory::destroy(ptr);
  }

private:
  uint32_t& m_test;
};

TYPED_TEST(unique_ptr, construction) {
  wn::memory::unique_ptr<TypeParam> pointer1;

  EXPECT_EQ(pointer1, nullptr);

  wn::testing::allocator allocator;

  {
    wn::memory::unique_ptr<TypeParam> pointer2(
        wn::memory::make_unique<TypeParam>(
            &allocator, static_cast<TypeParam>(1)));

    EXPECT_NE(pointer2, nullptr);
    ASSERT_NE(pointer2.get(), nullptr);
    EXPECT_EQ(*pointer2, static_cast<TypeParam>(1));

    TypeParam* value1 =
        allocator.construct<TypeParam>(static_cast<TypeParam>(2));
    wn::memory::unique_ptr<TypeParam> pointer3(&allocator, value1);

    EXPECT_NE(pointer3, nullptr);
    ASSERT_NE(pointer3.get(), nullptr);
    EXPECT_EQ(*pointer3, static_cast<TypeParam>(2));
  }
}

TYPED_TEST(unique_ptr, moving) {
  wn::testing::allocator allocator;

  {
    wn::memory::unique_ptr<TypeParam> pointer1(
        wn::memory::make_unique<TypeParam>(
            &allocator, static_cast<TypeParam>(1)));

    EXPECT_NE(pointer1, nullptr);

    wn::memory::unique_ptr<TypeParam> pointer2(std::move(pointer1));

    EXPECT_EQ(pointer1, nullptr);
    ASSERT_NE(pointer2, nullptr);
    EXPECT_EQ(*pointer2, static_cast<TypeParam>(1));

    {
      wn::memory::unique_ptr<TypeParam> pointer3;

      EXPECT_EQ(pointer3, nullptr);

      pointer3 = std::move(pointer2);

      EXPECT_EQ(pointer1, nullptr);
      EXPECT_EQ(pointer2, nullptr);
      ASSERT_NE(pointer3, nullptr);
      EXPECT_EQ(*pointer3, static_cast<TypeParam>(1));
    }

    EXPECT_EQ(pointer1, nullptr);
    EXPECT_EQ(pointer2, nullptr);
  }
}

TYPED_TEST(unique_ptr, swap) {
  wn::testing::allocator allocator;

  {
    wn::memory::unique_ptr<TypeParam> pointer1(
        wn::memory::make_unique<TypeParam>(
            &allocator, static_cast<TypeParam>(1)));
    wn::memory::unique_ptr<TypeParam> pointer2(
        wn::memory::make_unique<TypeParam>(
            &allocator, static_cast<TypeParam>(2)));

    ASSERT_NE(pointer1, nullptr);
    ASSERT_NE(pointer2, nullptr);
    EXPECT_EQ(*pointer1, static_cast<TypeParam>(1));
    EXPECT_EQ(*pointer2, static_cast<TypeParam>(2));

    const TypeParam* pointer1_raw = pointer1.get();
    const TypeParam* pointer2_raw = pointer2.get();

    pointer1.swap(pointer2);

    ASSERT_NE(pointer1, nullptr);
    ASSERT_NE(pointer2, nullptr);
    EXPECT_EQ(pointer1_raw, pointer2.get());
    EXPECT_EQ(pointer2_raw, pointer1.get());
    EXPECT_EQ(*pointer2, static_cast<TypeParam>(1));
    EXPECT_EQ(*pointer1, static_cast<TypeParam>(2));

    wn::core::swap(pointer1, pointer2);

    ASSERT_NE(pointer1, nullptr);
    ASSERT_NE(pointer2, nullptr);
    EXPECT_EQ(pointer1_raw, pointer1.get());
    EXPECT_EQ(pointer2_raw, pointer2.get());
    EXPECT_EQ(*pointer1, static_cast<TypeParam>(1));
    EXPECT_EQ(*pointer2, static_cast<TypeParam>(2));
  }
}

TYPED_TEST(unique_ptr, reset) {
  wn::testing::allocator allocator;

  {
    wn::memory::unique_ptr<TypeParam> pointer1(
        wn::memory::make_unique<TypeParam>(
            &allocator, static_cast<TypeParam>(1)));
    wn::memory::unique_ptr<TypeParam> pointer2(
        wn::memory::make_unique<TypeParam>(
            &allocator, static_cast<TypeParam>(2)));
    wn::memory::unique_ptr<TypeParam> pointer3(
        wn::memory::make_unique<TypeParam>(
            &allocator, static_cast<TypeParam>(3)));
    wn::memory::unique_ptr<TypeParam> pointer4(
        wn::memory::make_unique<TypeParam>(
            &allocator, static_cast<TypeParam>(4)));

    ASSERT_NE(pointer1, nullptr);
    ASSERT_NE(pointer2, nullptr);
    ASSERT_NE(pointer3, nullptr);
    ASSERT_NE(pointer4, nullptr);
    EXPECT_EQ(*pointer1, static_cast<TypeParam>(1));
    EXPECT_EQ(*pointer2, static_cast<TypeParam>(2));
    EXPECT_EQ(*pointer3, static_cast<TypeParam>(3));
    EXPECT_EQ(*pointer4, static_cast<TypeParam>(4));

    pointer1.reset();

    EXPECT_EQ(pointer1, nullptr);

    pointer2.reset(nullptr);

    EXPECT_EQ(pointer2, nullptr);

    TypeParam* value =
        allocator.construct<TypeParam>(static_cast<TypeParam>(5));

    pointer3.reset(&allocator, value);

    ASSERT_NE(pointer3, nullptr);
    EXPECT_EQ(*pointer3, static_cast<TypeParam>(5));
  }
}

TYPED_TEST(unique_ptr, release) {
  wn::testing::allocator allocator;

  {
    wn::memory::unique_ptr<TypeParam> pointer(
        wn::memory::make_unique<TypeParam>(
            &allocator, static_cast<TypeParam>(1)));

    ASSERT_NE(pointer, nullptr);
    EXPECT_EQ(*pointer, static_cast<TypeParam>(1));

    TypeParam* pointer_raw = pointer.get();
    TypeParam* value = pointer.release();

    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, static_cast<TypeParam>(1));
    EXPECT_EQ(value, pointer_raw);
    EXPECT_EQ(pointer, nullptr);

    allocator.destroy(value);
  }
}

struct b {
  int i;

private:
  friend struct a;

  b() : i(42) {}
};

struct a {
  wn::memory::unique_ptr<b> make_b(wn::memory::allocator* _allocator) const {
    return wn::memory::make_unique_delegated<b>(
        _allocator, [](void* _p) { return new (_p) b(); });
  }
};

TEST(unique_ptr, delegated) {
  wn::testing::allocator allocator;

  {
    wn::memory::unique_ptr<b> temp;

    temp = a().make_b(&allocator);

    EXPECT_EQ(temp->i, 42);
  }
}
