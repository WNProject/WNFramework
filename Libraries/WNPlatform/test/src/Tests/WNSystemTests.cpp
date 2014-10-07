#include "WNPlatform/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNPlatform/inc/WNSystem.h"
#endif

using namespace WNPlatform;

TEST(WNSystem, ProcessorCountTest) {
    WN_PRINTF("WNSystem: Processor Count Test\n");

    const WN_UINT8 physicalCoreCount = WNSystem::WNGetPhysicalCoreCount();
    const WN_UINT8 logicalThreadCount = WNSystem::WNGetLogicalThreadCount();

    WN_PRINTF("  Physical Core Count: %d\n", physicalCoreCount);
    WN_PRINTF("  Logical Thread Count: %d\n", logicalThreadCount);
}

TEST(WNSystem, HyperThreadedTest) {
    WN_PRINTF("WNSystem: Hyper Threaded Test\n");

    const WN_BOOL isHyperThreaded = WNSystem::WNIsHyperThreaded();

    WN_PRINTF("  Is Hyper Threaded: %s\n", isHyperThreaded ? "True" : "False");
}

TEST(WNSystem, EndiannessTest) {
    WN_PRINTF("WNSystem: Endianness Test\n");

    const WN_BOOL isLittleEndian = WNSystem::WNIsLittleEndian();
    const WN_BOOL isBigEndian = WNSystem::WNIsBigEndian();

    ASSERT_TRUE(!(isLittleEndian && isBigEndian));

    if (isLittleEndian) {
        WN_PRINTF("  Endianness: Little\n");
    } else if (isBigEndian) {
        WN_PRINTF("  Endianness: Big\n");
    }
}

TEST(WNSystem, FeaturesTest) {
    WN_PRINTF("WNSystem: Features Test\n");
    WN_PRINTF("  FPU: %s\n", WNSystem::WNHasFPU() ? "True" : "False");
    WN_PRINTF("  CMPXCHG8: %s\n", WNSystem::WNHasCMPXCHG8() ? "True" : "False");
    WN_PRINTF("  CMPXCHG16: %s\n", WNSystem::WNHasCMPXCHG16() ? "True" : "False");
    WN_PRINTF("  MMX: %s\n", WNSystem::WNHasMMX() ? "True" : "False");
    WN_PRINTF("  Extended MMX: %s\n", WNSystem::WNHasEMMI() ? "True" : "False");
    WN_PRINTF("  3DNow!: %s\n", WNSystem::WNHas3DNow() ? "True" : "False");
    WN_PRINTF("  3DNow!+: %s\n", WNSystem::WNHas3DNowPlus() ? "True" : "False");
    WN_PRINTF("  SSE: %s\n", WNSystem::WNHasSSE() ? "True" : "False");
    WN_PRINTF("  SSE2: %s\n", WNSystem::WNHasSSE2() ? "True" : "False");
    WN_PRINTF("  SSE3: %s\n", WNSystem::WNHasSSE3() ? "True" : "False");
    WN_PRINTF("  SSSE3: %s\n", WNSystem::WNHasSSSE3() ? "True" : "False");
    WN_PRINTF("  SSE4.1: %s\n", WNSystem::WNHasSSE41() ? "True" : "False");
    WN_PRINTF("  SSE4.2: %s\n", WNSystem::WNHasSSE42() ? "True" : "False");
    WN_PRINTF("  SSE4a: %s\n", WNSystem::WNHasSSE4a() ? "True" : "False");
    WN_PRINTF("  F16C: %s\n", WNSystem::WNHasF16C() ? "True" : "False");
    WN_PRINTF("  VEX Prefix: %s\n", WNSystem::WNHasVEXPrefix() ? "True" : "False");
    WN_PRINTF("  AES: %s\n", WNSystem::WNHasAES() ? "True" : "False");
    WN_PRINTF("  CLMUL: %s\n", WNSystem::WNHasCLMUL() ? "True" : "False");
    WN_PRINTF("  FMA3: %s\n", WNSystem::WNHasFMA3() ? "True" : "False");
    WN_PRINTF("  FMA4: %s\n", WNSystem::WNHasFMA4() ? "True" : "False");
    WN_PRINTF("  XOP: %s\n", WNSystem::WNHasXOP() ? "True" : "False");
    WN_PRINTF("  AVX: %s\n", WNSystem::WNHasAVX() ? "True" : "False");
    WN_PRINTF("  AVX2: %s\n", WNSystem::WNHasAVX2() ? "True" : "False");
}