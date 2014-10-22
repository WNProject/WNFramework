#include "WNContainers/test/inc/Common.h"
#include "WNContainers/inc/WNDeque.h"

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

TEST(WNDequeTests, CreationDefault) {
    WNContainers::WNDeque<WN_SIZE_T> intQueue;
    return;
}

TEST(WNDequeTests, GeneratorTest) {
    WNContainers::WNDeque<WN_SIZE_T> intQueue;
    intQueue.generate(intQueue.cbegin(), 1000, [](WN_SIZE_T i) {
        return(i);
    });
    for(WN_SIZE_T i = 0; i < 1000; ++i) {
        ASSERT_EQ(intQueue[i], i);
    }
    intQueue.generate(intQueue.cend(), 1000, [](WN_SIZE_T i) {
        return(i + 1000);
    });
    for(WN_SIZE_T i = 0; i < 2000; ++i) {
        ASSERT_EQ(intQueue[i], i);
    }
}

TEST(WNDequeTests, PushFrontTest) {
    WNContainers::WNDeque<WN_SIZE_T> intQueue;
    for(WN_SIZE_T i = 0; i < 10; ++i) {
        intQueue.push_front(10 - i - 1);
    }
    for(WN_SIZE_T i = 0; i < 10; ++i) {
        ASSERT_EQ(intQueue[i], i);
    }
}

TEST(WNDequeTests, PopBackTest) {
    WNContainers::WNDeque<WN_SIZE_T> intQueue;
    intQueue.generate(intQueue.cbegin(), 1000, [](WN_SIZE_T i) {
        return(i);
    });
    for(WN_SIZE_T i = 0; i < 10; ++i) {
        intQueue.pop_back();
    }
    ASSERT_EQ(intQueue.size(), 1000 - 10);
    for(WN_SIZE_T i = 0; i < 1000 - 10; ++i) {
        ASSERT_EQ(intQueue[i], i);
    }
}

TEST(WNDequeTests, PopFrontTest) {
    WNContainers::WNDeque<WN_SIZE_T> intQueue;
    intQueue.generate(intQueue.cbegin(), 1000, [](WN_SIZE_T i) {
        return(i);
    });
    for(WN_SIZE_T i = 0; i < 10; ++i) {
        intQueue.pop_front();
    }
    ASSERT_EQ(intQueue.size(), 1000 - 10);
    for(WN_SIZE_T i = 0; i < 1000 - 10; ++i) {
        ASSERT_EQ(intQueue[i], i + 10);
    }
}
