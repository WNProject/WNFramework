#include "WNConcurrency/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNConcurrency/inc/WNThread.h"
#endif

using namespace WNConcurrency;

static WN_VOID TestFunc1() {
    WN_PRINTF("               Start: TestFunc1\n");

    WNThreadSleep(RANDOM_TIME(0, 1000));

    WN_PRINTF("               End: TestFunc1\n");
}

static WN_UINT32 TestFunc2() {
    WN_PRINTF("               Start: TestFunc2\n");

    WNThreadSleep(RANDOM_TIME(0, 1000));

    WN_PRINTF("               End: TestFunc2\n");

    return(5);
}

static WN_VOID TestFunc3(WN_VOID* _input) {
    WN_UNUSED_ARG(_input);

    WN_PRINTF("               Start: TestFunc3\n");

    WNThreadSleep(RANDOM_TIME(0, 1000));

    WN_PRINTF("               End: TestFunc3\n");
}

static WN_VOID* TestFunc4(WN_VOID* _input) {
    WN_PRINTF("               Start: TestFunc4\n");

    WNThreadSleep(RANDOM_TIME(0, 1000));

    WN_PRINTF("               End: TestFunc4\n");

    return(_input);
}

static WN_VOID TestFunc5(WN_UINT32 _input) {
    WN_UNUSED_ARG(_input);

    WN_PRINTF("               Start: TestFunc5\n");

    WNThreadSleep(RANDOM_TIME(0, 1000));

    WN_PRINTF("               End: TestFunc5\n");
}

static WN_UINT32 TestFunc6(WN_UINT32 _input) {
    WN_PRINTF("               Start: TestFunc6\n");

    WNThreadSleep(RANDOM_TIME(0, 1000));

    WN_PRINTF("               End: TestFunc6\n");

    return(_input);
}

static WN_UINT32 TestFunc7(WN_UINT32* _input) {
    WN_PRINTF("               Start: TestFunc7\n");

    WNThreadSleep(RANDOM_TIME(0, 1000));

    WN_PRINTF("               End: TestFunc7\n");

    return(*_input);
}

static WN_UINT32* TestFunc8(WN_UINT32* _input) {
    WN_PRINTF("               Start: TestFunc8\n");

    WNThreadSleep(RANDOM_TIME(0, 1000));

    WN_PRINTF("               End: TestFunc8\n");

    return(_input);
}

static WN_VOID TestFunc9(WN_UINT32& _input) {
    WN_PRINTF("               Start: TestFunc9\n");

    _input = 1000;

    WNThreadSleep(RANDOM_TIME(0, 1000));

    WN_PRINTF("               End: TestFunc9\n");
}

class TestObject {
public:
    TestObject() :
        mTestVar(0) {
    }

    WN_VOID TestFunc1() {
        WN_PRINTF("               Start: Test::TestFunc1\n");

        mTestVar++;

        WNThreadSleep(RANDOM_TIME(0, 1000));

        WN_PRINTF("               End: Test::TestFunc1\n");
    }

    WN_UINT32 TestFunc2() {
        WN_PRINTF("               Start: Test::TestFunc2\n");

        WNThreadSleep(RANDOM_TIME(0, 1000));

        WN_PRINTF("               End: Test::TestFunc2\n");

        return(mTestVar + 1);
    }

public:
    WN_UINT32 mTestVar;
};

TEST(WNTheadValidation, StaticFunctionsCreationExecution) {
    WN_VOID* temp;
    WN_VOID* pointerTest = &temp;
    WN_UINT32 numberTest1 = 5;
    WN_UINT32 numberTest2 = 0;

    WNThread<WN_VOID> test1;
    WNThread<WN_UINT32> test2;
    WNThread<WN_VOID> test3;
    WNThread<WN_VOID*> test4;
    WNThread<WN_VOID> test5;
    WNThread<WN_UINT32> test6;
    WNThread<WN_UINT32> test7;
    WNThread<WN_UINT32*> test8;
    WNThread<WN_VOID> test9;

    test1.Initialize(&TestFunc1);
    test2.Initialize(&TestFunc2);
    test3.Initialize(&TestFunc3, pointerTest);
    test4.Initialize(&TestFunc4, pointerTest);
    test5.Initialize(&TestFunc5, numberTest1);
    test6.Initialize(&TestFunc6, numberTest1);
    test7.Initialize(&TestFunc7, &numberTest1);
    test8.Initialize(&TestFunc8, &numberTest1);
    test9.Initialize(&TestFunc9, numberTest2);

    test1.WaitForCompletion();
    test2.WaitForCompletion();
    test3.WaitForCompletion();
    test4.WaitForCompletion();
    test5.WaitForCompletion();
    test6.WaitForCompletion();
    test7.WaitForCompletion();
    test8.WaitForCompletion();
    test9.WaitForCompletion();

    WN_UINT32 test2Result;
    WN_VOID* test4Result;
    WN_UINT32 test6Result;
    WN_UINT32 test7Result;
    WN_UINT32* test8Result;

    const WN_BOOL test2GetReturn = test2.GetReturn(&test2Result);
    const WN_BOOL test4GetReturn = test4.GetReturn(&test4Result);
    const WN_BOOL test6GetReturn = test6.GetReturn(&test6Result);
    const WN_BOOL test7GetReturn = test7.GetReturn(&test7Result);
    const WN_BOOL test8GetReturn = test8.GetReturn(&test8Result);

    ASSERT_TRUE(test2GetReturn);
    ASSERT_TRUE(test4GetReturn);
    ASSERT_TRUE(test6GetReturn);
    ASSERT_TRUE(test7GetReturn);
    ASSERT_TRUE(test8GetReturn);

    ASSERT_EQ(5, test2Result);
    ASSERT_EQ(pointerTest, test4Result);
    ASSERT_EQ(numberTest1, test6Result);
    ASSERT_EQ(numberTest1, test7Result);
    ASSERT_EQ(&numberTest1, test8Result);
}

TEST(WNTheadValidation, MemberFunctionCreationExecution) {
    TestObject testObject1;
    TestObject testObject2;

    WNThread<WN_VOID> test1;
    WNThread<WN_UINT32> test2;

    test1.Initialize(&testObject1, &TestObject::TestFunc1);
    test2.Initialize(&testObject2, &TestObject::TestFunc2);

    test1.WaitForCompletion();
    test2.WaitForCompletion();

    WN_PRINTF("               testObject1::mTestVar: %d\n", testObject1.mTestVar);
    WN_PRINTF("               testObject2::mTestVar: %d\n", testObject2.mTestVar);

    WN_UINT32 test2Result;

    const WN_BOOL test2GetReturn = test2.GetReturn(&test2Result);

    ASSERT_TRUE(test2GetReturn);

    ASSERT_EQ(1, testObject1.mTestVar);
    ASSERT_EQ(0, testObject2.mTestVar);
    ASSERT_EQ(1, test2Result);
}
