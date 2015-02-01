// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMemory/test/inc/Common.h"
#include <memory>

struct dummy_intrusive : wn::intrusive_ptr_base {};
struct dummy_intrusive_child : dummy_intrusive {};

TEST(intrusive_ptr, construction) {
    wn::intrusive_ptr<dummy_intrusive> pointer1(wn::make_intrusive<dummy_intrusive>());

    ASSERT_NE(pointer1, wn_nullptr);
    ASSERT_NE(pointer1.get(), wn_nullptr);

    wn::intrusive_ptr<dummy_intrusive> pointer2(wn::make_intrusive<dummy_intrusive_child>());

    ASSERT_NE(pointer2, wn_nullptr);
    ASSERT_NE(pointer2.get(), wn_nullptr);

    dummy_intrusive* dummy1 = WN_NEW dummy_intrusive();
    wn::intrusive_ptr<dummy_intrusive> pointer3(dummy1);

    ASSERT_NE(pointer3, wn_nullptr);
    ASSERT_NE(pointer3.get(), wn_nullptr);

    dummy_intrusive_child* dummy2 = WN_NEW dummy_intrusive_child();
    wn::intrusive_ptr<dummy_intrusive> pointer4(dummy2);

    ASSERT_NE(pointer4, wn_nullptr);
    ASSERT_NE(pointer4.get(), wn_nullptr);
}

TEST(intrusive_ptr, use_count) {
    wn::intrusive_ptr<dummy_intrusive> pointer1(wn::make_intrusive<dummy_intrusive>());

    ASSERT_NE(pointer1, wn_nullptr);
    ASSERT_EQ(pointer1.use_count(), 1);
    ASSERT_NE(pointer1.get(), wn_nullptr);

    {
        wn::intrusive_ptr<dummy_intrusive> pointer2(pointer1);

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
    wn::intrusive_ptr<dummy_intrusive> pointer1(wn::make_intrusive<dummy_intrusive>());

    ASSERT_NE(pointer1, wn_nullptr);
    ASSERT_TRUE(pointer1.unique());
    ASSERT_NE(pointer1.get(), wn_nullptr);

    {
        wn::intrusive_ptr<dummy_intrusive> pointer2(pointer1);

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
    wn::intrusive_ptr<dummy_intrusive> pointer1(wn::make_intrusive<dummy_intrusive>());

    ASSERT_NE(pointer1, wn_nullptr);

    wn::intrusive_ptr<dummy_intrusive> pointer2(pointer1);

    ASSERT_NE(pointer1, wn_nullptr);
    ASSERT_NE(pointer2, wn_nullptr);
    ASSERT_EQ(pointer1.use_count(), 2);
    ASSERT_EQ(pointer2.use_count(), 2);
    ASSERT_FALSE(pointer1.unique());
    ASSERT_FALSE(pointer2.unique());
    ASSERT_EQ(pointer1, pointer2);

    {
        wn::intrusive_ptr<dummy_intrusive> pointer3;

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
    wn::intrusive_ptr<dummy_intrusive> pointer1(wn::make_intrusive<dummy_intrusive>());

    ASSERT_NE(pointer1, wn_nullptr);

    wn::intrusive_ptr<dummy_intrusive> pointer2(std::move(pointer1));

    ASSERT_EQ(pointer1, wn_nullptr);
    ASSERT_NE(pointer2, wn_nullptr);
    ASSERT_EQ(pointer1.use_count(), 0);
    ASSERT_EQ(pointer2.use_count(), 1);
    ASSERT_FALSE(pointer1.unique());
    ASSERT_TRUE(pointer2.unique());
    ASSERT_NE(pointer1, pointer2);

    {
        wn::intrusive_ptr<dummy_intrusive> pointer3;

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
    wn::intrusive_ptr<dummy_intrusive> pointer1(wn::make_intrusive<dummy_intrusive>());

    ASSERT_NE(pointer1, wn_nullptr);

    wn::intrusive_ptr<dummy_intrusive> pointer2(wn::make_intrusive<dummy_intrusive>());

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