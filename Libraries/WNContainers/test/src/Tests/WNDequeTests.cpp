// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/test/inc/Common.h"
#include "WNContainers/inc/WNDeque.h"

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

TEST(WNDequeTests, CreationDefault) {
    WNContainers::deque<wn_size_t> intQueue;
    return;
}

TEST(WNDequeTests, GeneratorTest) {
    WNContainers::deque<wn_size_t> intQueue;
    intQueue.generate(intQueue.cbegin(), 1000, [](wn_size_t i) {
        return(i);
    });
    for(wn_size_t i = 0; i < 1000; ++i) {
        ASSERT_EQ(intQueue[i], i);
    }
    intQueue.generate(intQueue.cend(), 1000, [](wn_size_t i) {
        return(i + 1000);
    });
    for(wn_size_t i = 0; i < 2000; ++i) {
        ASSERT_EQ(intQueue[i], i);
    }
}

TEST(WNDequeTests, PushFrontTest) {
    WNContainers::deque<wn_size_t> intQueue;
    for(wn_size_t i = 0; i < 10; ++i) {
        intQueue.push_front(10 - i - 1);
    }
    for(wn_size_t i = 0; i < 10; ++i) {
        ASSERT_EQ(intQueue[i], i);
    }
}

TEST(WNDequeTests, PopBackTest) {
    WNContainers::deque<wn_size_t> intQueue;
    intQueue.generate(intQueue.cbegin(), 1000, [](wn_size_t i) {
        return(i);
    });
    for(wn_size_t i = 0; i < 10; ++i) {
        intQueue.pop_back();
    }
    ASSERT_EQ(intQueue.size(), 1000 - 10);
    for(wn_size_t i = 0; i < 1000 - 10; ++i) {
        ASSERT_EQ(intQueue[i], i);
    }
}

TEST(WNDequeTests, PopFrontTest) {
    WNContainers::deque<wn_size_t> intQueue;
    intQueue.generate(intQueue.cbegin(), 1000, [](wn_size_t i) {
        return(i);
    });
    for(wn_size_t i = 0; i < 10; ++i) {
        intQueue.pop_front();
    }
    ASSERT_EQ(intQueue.size(), 1000 - 10);
    for(wn_size_t i = 0; i < 1000 - 10; ++i) {
        ASSERT_EQ(intQueue[i], i + 10);
    }
}
