// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMemory/test/inc/Common.h"

struct dummy_intrusive : wn::memory::intrusive_ptr_base {
    dummy_intrusive(const wn_int32 _value = 0) :
        m_test(_value) {
    }

    wn_int32 test() {
        return(m_test);
    }
    
    wn_int32 m_test;
};

struct dummy_intrusive_child : dummy_intrusive {};

TEST(intrusive_ptr, construction) {
    wn::memory::intrusive_ptr<dummy_intrusive> pointer1(wn::memory::make_intrusive<dummy_intrusive>(1));

    ASSERT_NE(pointer1, wn_nullptr);
    ASSERT_NE(pointer1.get(), wn_nullptr);

    wn::memory::intrusive_ptr<dummy_intrusive> pointer2(wn::memory::make_intrusive<dummy_intrusive_child>());

    ASSERT_NE(pointer2, wn_nullptr);
    ASSERT_NE(pointer2.get(), wn_nullptr);

    dummy_intrusive* dummy = wn::memory::construct<dummy_intrusive>(2);
    wn::memory::intrusive_ptr<dummy_intrusive> pointer3(dummy);

    ASSERT_NE(pointer3, wn_nullptr);
    ASSERT_NE(pointer3.get(), wn_nullptr);

    dummy_intrusive_child* dummy_child = wn::memory::construct<dummy_intrusive_child>();
    wn::memory::intrusive_ptr<dummy_intrusive> pointer4(dummy_child);

    ASSERT_NE(pointer4, wn_nullptr);
    ASSERT_NE(pointer4.get(), wn_nullptr);
    ASSERT_EQ(pointer1->test(), 1);
    ASSERT_EQ(pointer2->test(), 0);
    ASSERT_EQ(pointer3->test(), 2);
    ASSERT_EQ(pointer4->test(), 0);

    wn::memory::intrusive_ptr<dummy_intrusive> pointer5(pointer4.get(), wn_false);

    ASSERT_NE(pointer5, wn_nullptr);
    ASSERT_NE(pointer5.get(), wn_nullptr);
    ASSERT_EQ(pointer5.use_count(), 1);

    dummy_child->add_reference();

    ASSERT_EQ(pointer5.use_count(), 2);
}

TEST(intrusive_ptr, use_count) {
    wn::memory::intrusive_ptr<dummy_intrusive> pointer1(wn::memory::make_intrusive<dummy_intrusive>());

    ASSERT_NE(pointer1, wn_nullptr);
    ASSERT_EQ(pointer1.use_count(), 1);
    ASSERT_NE(pointer1.get(), wn_nullptr);

    {
        wn::memory::intrusive_ptr<dummy_intrusive> pointer2(pointer1);

        ASSERT_NE(pointer1, wn_nullptr);
        ASSERT_NE(pointer2, wn_nullptr);
        ASSERT_EQ(pointer1.use_count(), 2);
        ASSERT_EQ(pointer2.use_count(), 2);
        ASSERT_NE(pointer1.get(), wn_nullptr);
        ASSERT_NE(pointer2.get(), wn_nullptr);
    }

    ASSERT_NE(pointer1, wn_nullptr);
    ASSERT_EQ(pointer1.use_count(), 1);
    ASSERT_NE(pointer1.get(), wn_nullptr);
}

TEST(intrusive_ptr, unique) {
    wn::memory::intrusive_ptr<dummy_intrusive> pointer1(wn::memory::make_intrusive<dummy_intrusive>());

    ASSERT_NE(pointer1, wn_nullptr);
    ASSERT_TRUE(pointer1.unique());
    ASSERT_NE(pointer1.get(), wn_nullptr);

    {
        wn::memory::intrusive_ptr<dummy_intrusive> pointer2(pointer1);

        ASSERT_NE(pointer1, wn_nullptr);
        ASSERT_NE(pointer2, wn_nullptr);
        ASSERT_FALSE(pointer1.unique());
        ASSERT_FALSE(pointer2.unique());
        ASSERT_NE(pointer1.get(), wn_nullptr);
        ASSERT_NE(pointer2.get(), wn_nullptr);
    }

    ASSERT_NE(pointer1, wn_nullptr);
    ASSERT_TRUE(pointer1.unique());
    ASSERT_NE(pointer1.get(), wn_nullptr);
}

TEST(intrusive_ptr, copying) {
    wn::memory::intrusive_ptr<dummy_intrusive> pointer1(wn::memory::make_intrusive<dummy_intrusive>());

    ASSERT_NE(pointer1, wn_nullptr);

    wn::memory::intrusive_ptr<dummy_intrusive> pointer2(pointer1);

    ASSERT_NE(pointer1, wn_nullptr);
    ASSERT_NE(pointer2, wn_nullptr);
    ASSERT_EQ(pointer1.use_count(), 2);
    ASSERT_EQ(pointer2.use_count(), 2);
    ASSERT_FALSE(pointer1.unique());
    ASSERT_FALSE(pointer2.unique());
    ASSERT_EQ(pointer1, pointer2);

    {
        wn::memory::intrusive_ptr<dummy_intrusive> pointer3;

        ASSERT_EQ(pointer3, wn_nullptr);

        pointer3 = pointer2;

        ASSERT_NE(pointer1, wn_nullptr);
        ASSERT_NE(pointer2, wn_nullptr);
        ASSERT_NE(pointer3, wn_nullptr);
        ASSERT_EQ(pointer1.use_count(), 3);
        ASSERT_EQ(pointer2.use_count(), 3);
        ASSERT_EQ(pointer3.use_count(), 3);
        ASSERT_FALSE(pointer1.unique());
        ASSERT_FALSE(pointer2.unique());
        ASSERT_FALSE(pointer3.unique());
        ASSERT_EQ(pointer1, pointer2);
        ASSERT_EQ(pointer2, pointer3);
        ASSERT_EQ(pointer1, pointer3);
    }

    ASSERT_NE(pointer1, wn_nullptr);
    ASSERT_NE(pointer2, wn_nullptr);
    ASSERT_EQ(pointer1.use_count(), 2);
    ASSERT_EQ(pointer2.use_count(), 2);
    ASSERT_FALSE(pointer1.unique());
    ASSERT_FALSE(pointer2.unique());
    ASSERT_EQ(pointer1, pointer2);
}

TEST(intrusive_ptr, moving) {
    wn::memory::intrusive_ptr<dummy_intrusive> pointer1(wn::memory::make_intrusive<dummy_intrusive>());

    ASSERT_NE(pointer1, wn_nullptr);

    wn::memory::intrusive_ptr<dummy_intrusive> pointer2(std::move(pointer1));

    ASSERT_EQ(pointer1, wn_nullptr);
    ASSERT_NE(pointer2, wn_nullptr);
    ASSERT_EQ(pointer1.use_count(), 0);
    ASSERT_EQ(pointer2.use_count(), 1);
    ASSERT_FALSE(pointer1.unique());
    ASSERT_TRUE(pointer2.unique());
    ASSERT_NE(pointer1, pointer2);

    {
        wn::memory::intrusive_ptr<dummy_intrusive> pointer3;

        ASSERT_EQ(pointer3, wn_nullptr);

        pointer3 = std::move(pointer2);

        ASSERT_EQ(pointer1, wn_nullptr);
        ASSERT_EQ(pointer2, wn_nullptr);
        ASSERT_NE(pointer3, wn_nullptr);
        ASSERT_EQ(pointer1.use_count(), 0);
        ASSERT_EQ(pointer2.use_count(), 0);
        ASSERT_EQ(pointer3.use_count(), 1);
        ASSERT_FALSE(pointer1.unique());
        ASSERT_FALSE(pointer2.unique());
        ASSERT_TRUE(pointer3.unique());
        ASSERT_EQ(pointer1, pointer2);
        ASSERT_NE(pointer2, pointer3);
        ASSERT_NE(pointer1, pointer3);
    }

    ASSERT_EQ(pointer1, wn_nullptr);
    ASSERT_EQ(pointer2, wn_nullptr);
    ASSERT_EQ(pointer1.use_count(), 0);
    ASSERT_EQ(pointer2.use_count(), 0);
    ASSERT_FALSE(pointer1.unique());
    ASSERT_FALSE(pointer2.unique());
    ASSERT_EQ(pointer1, pointer2);
}

TEST(intrusive_ptr, swap) {
    wn::memory::intrusive_ptr<dummy_intrusive> pointer1(wn::memory::make_intrusive<dummy_intrusive>());
    wn::memory::intrusive_ptr<dummy_intrusive> pointer2(wn::memory::make_intrusive<dummy_intrusive>());

    ASSERT_NE(pointer1, wn_nullptr);
    ASSERT_NE(pointer2, wn_nullptr);

    const dummy_intrusive* pointer1_raw = pointer1.get();
    const dummy_intrusive* pointer2_raw = pointer2.get();

    pointer1.swap(pointer2);

    ASSERT_EQ(pointer1_raw, pointer2.get());
    ASSERT_EQ(pointer2_raw, pointer1.get());

    std::swap(pointer1, pointer2);

    ASSERT_EQ(pointer1_raw, pointer1.get());
    ASSERT_EQ(pointer2_raw, pointer2.get());
}

TEST(intrusive_ptr, reset) {
    wn::memory::intrusive_ptr<dummy_intrusive> pointer1(wn::memory::make_intrusive<dummy_intrusive>());
    wn::memory::intrusive_ptr<dummy_intrusive> pointer2(wn::memory::make_intrusive<dummy_intrusive>());
    wn::memory::intrusive_ptr<dummy_intrusive> pointer3(wn::memory::make_intrusive<dummy_intrusive>());
    wn::memory::intrusive_ptr<dummy_intrusive> pointer4(wn::memory::make_intrusive<dummy_intrusive>());

    ASSERT_NE(pointer1, wn_nullptr);
    ASSERT_NE(pointer2, wn_nullptr);
    ASSERT_NE(pointer3, wn_nullptr);
    ASSERT_NE(pointer4, wn_nullptr);

    pointer1.reset();
    pointer2.reset(wn_nullptr);

    dummy_intrusive* dummy1 = wn_nullptr;

    pointer3.reset(dummy1);

    dummy_intrusive_child* dummy2 = wn_nullptr;

    pointer4.reset(dummy2);

    ASSERT_EQ(pointer1, wn_nullptr);
    ASSERT_EQ(pointer1.use_count(), 0);
    ASSERT_EQ(pointer2, wn_nullptr);
    ASSERT_EQ(pointer2.use_count(), 0);
    ASSERT_EQ(pointer3, wn_nullptr);
    ASSERT_EQ(pointer3.use_count(), 0);
    ASSERT_EQ(pointer4, wn_nullptr);
    ASSERT_EQ(pointer4.use_count(), 0);
}

TEST(intrusive_ptr, release) {
    wn::memory::intrusive_ptr<dummy_intrusive> pointer(wn::memory::make_intrusive<dummy_intrusive>());

    ASSERT_NE(pointer, wn_nullptr);

    const dummy_intrusive* dummy = pointer.release();

    ASSERT_NE(dummy, wn_nullptr);
    ASSERT_EQ(dummy->reference_count(), 1);
    ASSERT_EQ(pointer, wn_nullptr);
    ASSERT_EQ(pointer.use_count(), 0);

    wn::memory::destroy(dummy);
}