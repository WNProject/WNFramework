// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/test/inc/Common.h"
#include "WNContainers/inc/WNAutoCleaner.h"

wn_void WNObjectDeleter(wn_int32*& _val) {
    *_val = 1;
}

wn_void WNIntDeleter(wn_int32& _val) {
    _val = 1;
}

wn_void CleanTest(wn_int32* val) {
    WNContainers::WNAutoCleaner<wn_int32*, WNObjectDeleter> mObj(val);

    mObj.Detach();
}

wn_void DirtyTest(wn_int32* val) {
    WNContainers::WNAutoCleaner<wn_int32*, WNObjectDeleter> mObj(val);
}

wn_void MixedTest(wn_int32* val, wn_bool _succeed) {
    WNContainers::WNAutoCleaner<wn_int32*, WNObjectDeleter> mObj(val);

    if (!_succeed) {
        return;
    }

    mObj.Detach();
}

TEST(WNAutoCleanerValidation, BasicSuccessUsage) {
    wn_int32 val = 0;

    CleanTest(&val);

    ASSERT_EQ(val, 0);
}

TEST(WNAutoCleanerValidation, BasicFailureUsage) {
    wn_int32 val = 0;

    DirtyTest(&val);

    ASSERT_EQ(val, 1);
}

TEST(WNAutoCleanerValidation, MixedUsage) {
    wn_int32 val = 0;

    MixedTest(&val, wn_true);

    ASSERT_EQ(val, 0);

    MixedTest(&val, wn_false);

    ASSERT_EQ(val, 1);
}

TEST(WNAutoCleanerValidation, ReadUsage) {
    wn_int32 val = 10;
    WNContainers::WNAutoCleaner<wn_int32, WNIntDeleter> mObj(val);

    ASSERT_EQ(mObj, 10);
}

TEST(WNAutoCleanerValidation, WriteUsage) {
    wn_int32 val = 10;
    WNContainers::WNAutoCleaner<wn_int32, WNIntDeleter> mObj(val);

    val = 5;

    ASSERT_EQ(mObj, 5);

    mObj = 10;
    ASSERT_EQ(val, 10);
    ASSERT_EQ(mObj, 10);
}
