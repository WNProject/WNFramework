// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/test/inc/Common.h"
#include "WNContainers/inc/WNDynamicArray.h"

class WNMockAllocator : public wn::memory::default_allocator {
public:
    WNMockAllocator(): mNumAllocations(0) {
    }
    virtual wn::memory::allocation_pair allocate(const wn_size_t _size, const wn_size_t _count) {
        ++mNumAllocations;
        return(wn::memory::default_allocator::allocate(_size, _count));
    }
    virtual wn::memory::allocation_pair reallocate(wn_void* _position, const wn_size_t _size, const wn_size_t _count) {
        return(wn::memory::default_allocator::reallocate(_position, _size, _count));
    }
    virtual wn::memory::allocation_pair allocate_for_resize(const wn_size_t _size, const wn_size_t _count, const wn_size_t _oldCount) {
        ++mNumAllocations;
        return(wn::memory::default_allocator::allocate_for_resize(_size, _count, _oldCount));
    }
    virtual wn_void deallocate(wn_void* _pos) {
        if(_pos) {
            --mNumAllocations;
        }
        return(wn::memory::default_allocator::deallocate(_pos));
    }
    wn_size_t mNumAllocations;
};

class WNMockGreedyAllocator : public wn::memory::default_allocator {
public:
    WNMockGreedyAllocator(): mNumAllocations(0), mTotalAllocations(0) {
    }

    virtual wn::memory::allocation_pair allocate(const wn_size_t _size, const wn_size_t _count) {
        ++mNumAllocations;
        ++mTotalAllocations;
        return(wn::memory::default_allocator::allocate(_size, _count * 2));
    }
    virtual wn::memory::allocation_pair reallocate(wn_void* _position, const wn_size_t _size, const wn_size_t _count) {
        ++mTotalAllocations;
        return(wn::memory::default_allocator::reallocate(_position, _size, _count * 2));
    }
    virtual wn::memory::allocation_pair allocate_for_resize(const wn_size_t _size, const wn_size_t _count, const wn_size_t _oldCount) {
        return(allocate(_size, _count));
    }

    virtual wn_void deallocate(wn_void* _pos) {
        if(_pos) {
            --mNumAllocations;
        }
        return(wn::memory::default_allocator::deallocate(_pos));
    }
    wn_size_t mNumAllocations;
    wn_size_t mTotalAllocations;
};

TEST(WNDynamicArrayTests, Creation) {
    WNContainers::dynamic_array<wn_size_t> intArray(10);
}

TEST(WNDynamicArrayTests, CreationDefault) {
    WNContainers::dynamic_array<wn_size_t> intArray(10, 5);
    for(wn_size_t i = 0; i < 10; ++i) {
        ASSERT_EQ(intArray[i], 5);
    }
}

TEST(WNDynamicArrayTests, CustomAllocator) {
    WNMockAllocator mAllocator;
    {
        WNContainers::dynamic_array<wn_size_t> intArray(10, &mAllocator);
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, CustomAllocatorDefault) {
    WNMockAllocator mAllocator;
    {
        WNContainers::dynamic_array<wn_size_t> intArray(10, 5, &mAllocator);
        for(wn_size_t i = 0; i < 10; ++i) {
            ASSERT_EQ(intArray[i], 5);
        }
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, PushBackTest) {
    WNMockAllocator mAllocator;
    {
        WNContainers::dynamic_array<wn_size_t> intArray(10, 5, &mAllocator);
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
        WNContainers::dynamic_array<wn_size_t> intArray(10, 5, &mAllocator);
        ASSERT_EQ(intArray.size(), 10);
        intArray.pop_back();
        ASSERT_EQ(intArray.size(), 9);

    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, GeneratorTest) {
    WNMockAllocator mAllocator;
    {
        WNContainers::dynamic_array<wn_size_t> intArray(&mAllocator);
        ASSERT_EQ(intArray.size(), 0);
        intArray.generate(intArray.begin(), 100, [](wn_size_t i) {
            return(i);
        });
        ASSERT_EQ(intArray.size(), 100);
        for(wn_size_t i = 0; i < 100; ++i) {
            ASSERT_EQ(intArray[i], i);
        }
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, InsertTest) {
    WNMockAllocator mAllocator;
    {
        WNContainers::dynamic_array<wn_size_t> intArray(&mAllocator);
        WNContainers::dynamic_array<wn_size_t> intArray2(&mAllocator);
        ASSERT_EQ(intArray.size(), 0);
        intArray.generate(intArray.begin(), 100, [](wn_size_t i) {
            return(i);
        });
        ASSERT_EQ(intArray.size(), 100);
        for(wn_size_t i = 0; i < 100; ++i) {
            ASSERT_EQ(intArray[i], i);
        }
        intArray2.insert(intArray2.begin(), intArray.begin(), intArray.begin() + 10);
        ASSERT_EQ(intArray2.size(), 10);
        for(wn_size_t i = 0; i < 10; ++i) {
            ASSERT_EQ(intArray2[i], i);
        }
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, EraseOne) {
    WNMockAllocator mAllocator;
    {
        WNContainers::dynamic_array<wn_size_t> intArray(&mAllocator);
        ASSERT_EQ(intArray.size(), 0);
        intArray.generate(intArray.begin(), 100, [](wn_size_t i) {
            return(i);
        });
        ASSERT_EQ(intArray.size(), 100);
        for(wn_size_t i = 0; i < 100; ++i) {
            ASSERT_EQ(intArray[i], i);
        }
        intArray.erase(intArray.begin());
        ASSERT_EQ(intArray.size(), 99);
        for(wn_size_t i = 0; i < 99; ++i) {
            ASSERT_EQ(intArray[i], i + 1);
        }
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, EraseRange) {
    WNMockAllocator mAllocator;
    {
        WNContainers::dynamic_array<wn_size_t> intArray(&mAllocator);
        ASSERT_EQ(intArray.size(), 0);
        intArray.generate(intArray.begin(), 100, [](wn_size_t i) {
            return(i);
        });
        ASSERT_EQ(intArray.size(), 100);
        for(wn_size_t i = 0; i < 100; ++i) {
            ASSERT_EQ(intArray[i], i);
        }
        intArray.erase(intArray.begin(), intArray.begin() + 10);
        ASSERT_EQ(intArray.size(), 90);
        for(wn_size_t i = 0; i < 90; ++i) {
            ASSERT_EQ(intArray[i], i + 10);
        }
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, EraseRangeFromMiddle) {
    WNMockAllocator mAllocator;
    {
        WNContainers::dynamic_array<wn_size_t> intArray(&mAllocator);
        ASSERT_EQ(intArray.size(), 0);
        intArray.generate(intArray.begin(), 100, [](wn_size_t i) {
            return(i);
        });
        ASSERT_EQ(intArray.size(), 100);
        for(wn_size_t i = 0; i < 100; ++i) {
            ASSERT_EQ(intArray[i], i);
        }
        intArray.erase(intArray.begin()+ 10, intArray.begin() + 20);
        ASSERT_EQ(intArray.size(), 90);
        for(wn_size_t i = 0; i < 10; ++i) {
            ASSERT_EQ(intArray[i], i);
        }
        for(wn_size_t i = 10; i < 90; ++i) {
            ASSERT_EQ(intArray[i], i + 10);
        }
    }
    ASSERT_EQ(mAllocator.mNumAllocations, 0);
}

TEST(WNDynamicArrayTests, TestOverallocation) {
    WNMockGreedyAllocator mAllocator;
    {
         WNContainers::dynamic_array<wn_size_t> intArray(100, 0, &mAllocator);
         ASSERT_EQ(mAllocator.mTotalAllocations, 1);
         for(wn_size_t i = 0; i < 100; ++i) {
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
         WNContainers::dynamic_array<wn_size_t> intArray(&mAllocator);
         WNContainers::dynamic_array<wn_size_t> intArrayT(100, &mAllocator2);
         intArrayT.generate(intArrayT.begin(), 100, [](wn_size_t i) {
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
    WNContainers::dynamic_array<wn_size_t> intArray;
    intArray.push_back(10);
    intArray.push_back(20);
    intArray.swap_elements(intArray.begin(), intArray.begin() + 1);
    ASSERT_EQ(intArray[0], 20);
    ASSERT_EQ(intArray[1], 10);
}
