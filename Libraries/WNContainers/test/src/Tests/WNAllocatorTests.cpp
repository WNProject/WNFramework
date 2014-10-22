#include "WNContainers/test/inc/Common.h"
#include "WNContainers/inc/WNAllocator.h"

TEST(WNAllocatorValidation, BasicUsage) {
    WNContainers::WNDefaultAllocator defaultAllocator;
    WNContainers::WNAllocationPair pair = defaultAllocator.Allocate(1, 1);
    ASSERT_GE(pair.count, 1u);
    ASSERT_NE(pair.m_pLocation, nullptr);
    char* c = reinterpret_cast<char*>(pair.m_pLocation);
    c[0] = 'a'; //Just to make sure this is a "real" memory location
    defaultAllocator.Free(pair.m_pLocation);
}

TEST(WNAllocatorValidation, MultipleItems) {
    WNContainers::WNDefaultAllocator defaultAllocator;
    WNContainers::WNAllocationPair pair = defaultAllocator.Allocate(1, 10);
    ASSERT_GE(pair.count, 10u);
    char* c = reinterpret_cast<char*>(pair.m_pLocation);
    for(WN_SIZE_T i = 0; i < 10; ++i) {
        c[i] = 'a';
    }
    ASSERT_NE(pair.m_pLocation, nullptr);
    defaultAllocator.Free(pair.m_pLocation);
}

TEST(WNAllocatorValidation, PageSize) {
    WNContainers::WNDefaultAllocator defaultAllocator;
    WNContainers::WNAllocationPair pair = defaultAllocator.Allocate(4096, 1);
    ASSERT_GE(pair.count, 1u);
    char* c = reinterpret_cast<char*>(pair.m_pLocation);
    for(WN_SIZE_T i = 0; i < 4096; ++i) {
        c[i] = 'a';
    }
    ASSERT_NE(pair.m_pLocation, nullptr);
    defaultAllocator.Free(pair.m_pLocation);
}

TEST(WNAllocatorValidation, Reallocate) {
    WNContainers::WNDefaultAllocator defaultAllocator;
    WNContainers::WNAllocationPair pair = defaultAllocator.Allocate(1, 1);
    ASSERT_GE(pair.count, 1u);
    ASSERT_NE(pair.m_pLocation, nullptr);
    char* c = reinterpret_cast<char*>(pair.m_pLocation);
    c[0] = 'a'; //Just to make sure this is a "real" memory location
    pair = defaultAllocator.Reallocate(c, 1, 100);
    c = reinterpret_cast<char*>(pair.m_pLocation);
    ASSERT_EQ(c[0], 'a');
    for(WN_SIZE_T i = 1; i < 100; ++i) {
        c[i] = 'a';
    }
    ASSERT_NE(pair.m_pLocation, nullptr);
    defaultAllocator.Free(pair.m_pLocation);
}

TEST(WNAllocatorValidation, PageReallocate) {
    WNContainers::WNDefaultAllocator defaultAllocator;
    WNContainers::WNAllocationPair pair = defaultAllocator.Allocate(4096, 1);
    ASSERT_GE(pair.count, 1u);
    ASSERT_NE(pair.m_pLocation, nullptr);
    char* c = reinterpret_cast<char*>(pair.m_pLocation);
    for(WN_SIZE_T i = 0; i < 4096; ++i) {
        c[i] = 'a';
    }
    pair = defaultAllocator.Reallocate(c, 4096, 2);
    c = reinterpret_cast<char*>(pair.m_pLocation);
    for(WN_SIZE_T i = 0; i < 4096; ++i) {
        ASSERT_EQ(c[i],'a');
    }
    for(WN_SIZE_T i = 0; i < 4096; ++i) {
        c[i+4096] = 'a';
    }
    ASSERT_NE(pair.m_pLocation, nullptr);
    defaultAllocator.Free(pair.m_pLocation);
}
 