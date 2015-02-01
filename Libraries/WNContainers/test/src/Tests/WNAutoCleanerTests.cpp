#include "WNContainers/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNContainers/inc/WNAutoCleaner.h"
#endif

WN_VOID WNObjectDeleter(WN_INT32*& _val) {
    *_val = 1;
}

WN_VOID WNIntDeleter(WN_INT32& _val) {
    _val = 1;
}

WN_VOID CleanTest(WN_INT32* val) {
    WNContainers::WNAutoCleaner<WN_INT32*, WNObjectDeleter> mObj(val);

    mObj.Detach();
}

WN_VOID DirtyTest(WN_INT32* val) {
    WNContainers::WNAutoCleaner<WN_INT32*, WNObjectDeleter> mObj(val);
}

WN_VOID MixedTest(WN_INT32* val, WN_BOOL _succeed) {
    WNContainers::WNAutoCleaner<WN_INT32*, WNObjectDeleter> mObj(val);

    if (!_succeed) {
        return;
    }

    mObj.Detach();
}

TEST(WNAutoCleanerValidation, BasicSuccessUsage) {
    WN_INT32 val = 0;

    CleanTest(&val);

    ASSERT_EQ(val, 0);
}

TEST(WNAutoCleanerValidation, BasicFailureUsage) {
    WN_INT32 val = 0;

    DirtyTest(&val);

    ASSERT_EQ(val, 1);
}

TEST(WNAutoCleanerValidation, MixedUsage) {
    WN_INT32 val = 0;

    MixedTest(&val, WN_TRUE);

    ASSERT_EQ(val, 0);

    MixedTest(&val, WN_FALSE);

    ASSERT_EQ(val, 1);
}

TEST(WNAutoCleanerValidation, ReadUsage) {
    WN_INT32 val = 10;
    WNContainers::WNAutoCleaner<WN_INT32, WNIntDeleter> mObj(val);

    ASSERT_EQ(mObj, 10);
}

TEST(WNAutoCleanerValidation, WriteUsage) {
    WN_INT32 val = 10;
    WNContainers::WNAutoCleaner<WN_INT32, WNIntDeleter> mObj(val);

    val = 5;

    ASSERT_EQ(mObj, 5);

    mObj = 10;
    ASSERT_EQ(val, 10);
    ASSERT_EQ(mObj, 10);
}
