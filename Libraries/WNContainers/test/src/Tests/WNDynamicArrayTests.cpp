#include "WNContainers/test/inc/Common.h"
#include "WNContainers/inc/WNDynamicArray.h"

class WNMockAllocator: public WNContainers::WNDefaultAllocator {
public:
    WNMockAllocator(): mNumAllocations(0) {   
    }
    virtual WNContainers::WNAllocationPair Allocate(WN_SIZE_T _size, WN_SIZE_T _count) {
        ++mNumAllocations;
        return(WNContainers::WNDefaultAllocator::Allocate(_size, _count));
    }
    virtual WNContainers::WNAllocationPair Reallocate(WN_VOID* _position, WN_SIZE_T _size, WN_SIZE_T _count) {
        return(WNContainers::WNDefaultAllocator::Reallocate(_position, _size, _count));
    }
    virtual WNContainers::WNAllocationPair AllocateForResize(WN_SIZE_T _size, WN_SIZE_T _count, WN_SIZE_T _oldCount) {
        ++mNumAllocations;
        return(WNContainers::WNDefaultAllocator::AllocateForResize(_size, _count, _oldCount));
    }
    virtual WN_VOID Free(WN_VOID* _pos) {
        if(_pos) {
            --mNumAllocations;
        }
        return(WNContainers::WNDefaultAllocator::Free(_pos));
    }
    WN_SIZE_T mNumAllocations;
};

class WNMockGreedyAllocator: public WNContainers::WNDefaultAllocator {
public:
    WNMockGreedyAllocator(): mNumAllocations(0), mTotalAllocations(0) {   
    }

    virtual WNContainers::WNAllocationPair Allocate(WN_SIZE_T _size, WN_SIZE_T _count) {
        ++mNumAllocations;
        ++mTotalAllocations;
        return(WNContainers::WNDefaultAllocator::Allocate(_size, _count * 2));
    }
    virtual WNContainers::WNAllocationPair Reallocate(WN_VOID* _position, WN_SIZE_T _size, WN_SIZE_T _count) {
        ++mTotalAllocations;
        return(WNContainers::WNDefaultAllocator::Reallocate(_position, _size, _count * 2));
    }
    virtual WNContainers::WNAllocationPair AllocateForResize(WN_SIZE_T _size, WN_SIZE_T _count, WN_SIZE_T _oldCount) {
        return(Allocate(_size, _count));
    }

    virtual WN_VOID Free(WN_VOID* _pos) {
        if(_pos) {
            --mNumAllocations;
        }
        return(WNContainers::WNDefaultAllocator::Free(_pos));
    }
    WN_SIZE_T mNumAllocations;
    WN_SIZE_T mTotalAllocations;
};

TEST(WNDynamicArrayTests, Creation) {
    WNContainers::WNDynamicArray<WN_SIZE_T> intArray(10);
}

TEST(WNDynamicArrayTests, CreationDefault) {
    WNContainers::WNDynamicArray<WN_SIZE_T> intArray(10, 5);
    for(WN_SIZE_T i = 0; i < 10; ++i) {
        ASSERT_EQ(intArray[i], 5);
    }
}

TEST(WNDynamicArrayTests, CustomAllocator) {
    WNMockAllocator mAllocator;
    {
        WNContainers::WNDynamicArray<WN_SIZE_T> intArray(10, &mAllocator);
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, CustomAllocatorDefault) {
    WNMockAllocator mAllocator;
    {
        WNContainers::WNDynamicArray<WN_SIZE_T> intArray(10, 5, &mAllocator);
        for(WN_SIZE_T i = 0; i < 10; ++i) {
            ASSERT_EQ(intArray[i], 5);
        }
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, PushBackTest) {
    WNMockAllocator mAllocator;
    {
        WNContainers::WNDynamicArray<WN_SIZE_T> intArray(10, 5, &mAllocator);
        ASSERT_EQ(intArray.size(), 10);
        intArray.push_back(25);
        ASSERT_EQ(intArray.size(), 11);
        ASSERT_EQ(intArray[10], 25);
        
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, PopBackTest) {
    WNMockAllocator mAllocator;
    {
        WNContainers::WNDynamicArray<WN_SIZE_T> intArray(10, 5, &mAllocator);
        ASSERT_EQ(intArray.size(), 10);
        intArray.pop_back();
        ASSERT_EQ(intArray.size(), 9);
        
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, GeneratorTest) {
    WNMockAllocator mAllocator;
    {
        WNContainers::WNDynamicArray<WN_SIZE_T> intArray(&mAllocator);
        ASSERT_EQ(intArray.size(), 0);
        intArray.generate(intArray.begin(), 100, [](WN_SIZE_T i) {
            return(i);
        });
        ASSERT_EQ(intArray.size(), 100);
        for(WN_SIZE_T i = 0; i < 100; ++i) {
            ASSERT_EQ(intArray[i], i);
        }   
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, InsertTest) {
    WNMockAllocator mAllocator;
    {
        WNContainers::WNDynamicArray<WN_SIZE_T> intArray(&mAllocator);
        WNContainers::WNDynamicArray<WN_SIZE_T> intArray2(&mAllocator);
        ASSERT_EQ(intArray.size(), 0);
        intArray.generate(intArray.begin(), 100, [](WN_SIZE_T i) {
            return(i);
        });
        ASSERT_EQ(intArray.size(), 100);
        for(WN_SIZE_T i = 0; i < 100; ++i) {
            ASSERT_EQ(intArray[i], i);
        }
        intArray2.insert(intArray2.begin(), intArray.begin(), intArray.begin() + 10);
        ASSERT_EQ(intArray2.size(), 10);
        for(WN_SIZE_T i = 0; i < 10; ++i) {
            ASSERT_EQ(intArray2[i], i);
        }
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, EraseOne) {
    WNMockAllocator mAllocator;
    {
        WNContainers::WNDynamicArray<WN_SIZE_T> intArray(&mAllocator);
        ASSERT_EQ(intArray.size(), 0);
        intArray.generate(intArray.begin(), 100, [](WN_SIZE_T i) {
            return(i);
        });
        ASSERT_EQ(intArray.size(), 100);
        for(WN_SIZE_T i = 0; i < 100; ++i) {
            ASSERT_EQ(intArray[i], i);
        }
        intArray.erase(intArray.begin());
        ASSERT_EQ(intArray.size(), 99);
        for(WN_SIZE_T i = 0; i < 99; ++i) {
            ASSERT_EQ(intArray[i], i + 1);
        }
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, EraseRange) {
    WNMockAllocator mAllocator;
    {
        WNContainers::WNDynamicArray<WN_SIZE_T> intArray(&mAllocator);
        ASSERT_EQ(intArray.size(), 0);
        intArray.generate(intArray.begin(), 100, [](WN_SIZE_T i) {
            return(i);
        });
        ASSERT_EQ(intArray.size(), 100);
        for(WN_SIZE_T i = 0; i < 100; ++i) {
            ASSERT_EQ(intArray[i], i);
        }
        intArray.erase(intArray.begin(), intArray.begin() + 10);
        ASSERT_EQ(intArray.size(), 90);
        for(WN_SIZE_T i = 0; i < 90; ++i) {
            ASSERT_EQ(intArray[i], i + 10);
        }
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, EraseRangeFromMiddle) {
    WNMockAllocator mAllocator;
    {
        WNContainers::WNDynamicArray<WN_SIZE_T> intArray(&mAllocator);
        ASSERT_EQ(intArray.size(), 0);
        intArray.generate(intArray.begin(), 100, [](WN_SIZE_T i) {
            return(i);
        });
        ASSERT_EQ(intArray.size(), 100);
        for(WN_SIZE_T i = 0; i < 100; ++i) {
            ASSERT_EQ(intArray[i], i);
        }
        intArray.erase(intArray.begin()+ 10, intArray.begin() + 20);
        ASSERT_EQ(intArray.size(), 90);
        for(WN_SIZE_T i = 0; i < 10; ++i) {
            ASSERT_EQ(intArray[i], i);
        }
        for(WN_SIZE_T i = 10; i < 90; ++i) {
            ASSERT_EQ(intArray[i], i + 10);
        }
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, TestOverallocation) {
    WNMockGreedyAllocator mAllocator;
    {
         WNContainers::WNDynamicArray<WN_SIZE_T> intArray(100, 0, &mAllocator);
         ASSERT_EQ(mAllocator.mTotalAllocations, 1);
         for(WN_SIZE_T i = 0; i < 100; ++i) {
            intArray.push_back(i);
            ASSERT_EQ(mAllocator.mTotalAllocations, 1);
         }
         intArray.push_back(101);
         ASSERT_EQ(mAllocator.mTotalAllocations, 2);
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, TestOverallocationInsertion) {
    WNMockGreedyAllocator mAllocator;
    WNMockAllocator mAllocator2;
    {
         WNContainers::WNDynamicArray<WN_SIZE_T> intArray(&mAllocator);
         WNContainers::WNDynamicArray<WN_SIZE_T> intArrayT(100, &mAllocator2);
         intArrayT.generate(intArrayT.begin(), 100, [](WN_SIZE_T i) {
            return(i);
         });
         ASSERT_EQ(mAllocator.mTotalAllocations, 0);
         intArray.insert(intArray.end(), intArrayT.begin(), intArrayT.end());
         ASSERT_EQ(intArray.size(), 100);
         ASSERT_EQ(mAllocator.mTotalAllocations, 1);
         intArray.insert(intArray.end(), intArrayT.begin(), intArrayT.end());
         ASSERT_EQ(intArray.size(), 200);
         ASSERT_EQ(mAllocator.mTotalAllocations, 1);
         intArray.insert(intArray.end(), intArrayT.begin(), intArrayT.end());
         ASSERT_EQ(intArray.size(), 300);
         ASSERT_EQ(mAllocator.mTotalAllocations, 2);
         for(int i = 0; i < 300; ++i) {
            ASSERT_EQ(intArray[i], i%100);
         }
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
    ASSERT_EQ(mAllocator2.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, SwapElementTest) {
    WNContainers::WNDynamicArray<WN_SIZE_T> intArray;
    intArray.push_back(10);
    intArray.push_back(20);
    intArray.swap_elements(intArray.begin(), intArray.begin() + 1);
    ASSERT_EQ(intArray[0], 20);
    ASSERT_EQ(intArray[1], 10);
}
