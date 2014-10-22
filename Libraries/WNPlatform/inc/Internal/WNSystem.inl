// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_SYSTEM_INL__
#define __WN_PLATFORM_INTERNAL_SYSTEM_INL__

#ifndef __WN_PLATFORM_SYSTEM_H__
    #error "WNSystem.inl should never be included directly. Please include WNSystem.h instead"
#endif

#include "WNCore/inc/WNConfig.h"
#include "WNCore/inc/WNMacros.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
#elif defined _WN_POSIX 
    #include <unistd.h>
    #include <stdio.h>
    #include <sys/sysinfo.h>
#endif

#ifdef _WN_X86
    #ifdef _WN_MSVC
        #if _WN_MSVC_MAJOR < 16
            #pragma warning (push)
            #pragma warning (disable: 4985)
        #endif

        #include <intrin.h>

        #if _WN_MSVC_MAJOR < 16
            #pragma warning (pop)
        #endif

        #ifdef _WN_WINDOWS
            #ifndef XSTATE_MASK_GSSE
                #define __WN_XSTATE_MASK_GSSE (1i64 << 2)
            #else
                #define __WN_XSTATE_MASK_GSSE XSTATE_MASK_GSSE
            #endif
        #endif
    #elif defined _WN_GCC || defined _WN_CLANG
        #define __WN_XCR_XFEATURE_ENABLED_MASK 0
    #endif
#endif

namespace WNPlatform {
    namespace __WNInternal {
        #ifdef _WN_ANDROID
            ssize_t getline(char** lineptr, size_t* n, FILE* stream) {
              size_t length = 0;
              char* c = fgetln(stream, &length); 
              if(WN_NULL == c || WN_NULL == lineptr)
              {
                  return(-1);
              }
              if(*lineptr == NULL)
              {
                *lineptr = static_cast<char*>(malloc(length + 1));
              } else {
                if(length + 1 > *n)
                {
                    *lineptr = static_cast<char*>(realloc(*lineptr, length + 1));
                }
              }

              *n = length;
              memcpy(*lineptr, c, length);
              (*lineptr)[length] = '\0';
              *n += 1;
              return(*n);   
            }
        #endif
        #ifdef _WN_X86
            #ifdef _WN_MSVC
                WN_FORCE_INLINE WN_BOOL __WNHasCPUID() {
                    #ifdef _WN_32_BIT
                        WN_UINT32 result = 0;

                        __asm {
                            pushfd
                            pop eax
                            mov ecx, eax
                            xor eax, 0x200000
                            push eax
                            popfd
                            pushfd
                            pop eax
                            xor eax, ecx
                            mov result, eax
                            push ecx
                            popfd
                        }

                        return(result != 0);
                    #elif defined _WN_64_BIT
                        return(WN_TRUE);
                    #endif
                }

                WN_FORCE_INLINE WN_VOID __WNCPUID(WN_INT32 _cpuInfo[4], WN_INT32 _infoType) {
                    __cpuid(_cpuInfo, _infoType);
                }
            #elif defined _WN_GCC || defined _WN_CLANG
                WN_FORCE_INLINE WN_BOOL __WNHasCPUID() {
                    #ifdef _WN_32_BIT
                        WN_UINT32 result = 0;

                        __asm__ __volatile__ (
                            "pushfl\n"
                            "pop %%eax\n"
                            "mov %%eax, %%ecx\n"
                            "xor $0x200000, %%eax\n"
                            "push %%eax\n"
                            "popfl\n"
                            "pushfl\n"
                            "pop %%eax\n"
                            "xor %%ecx, %%eax\n"
                            "mov %%eax, %0\n"
                            "push %%ecx\n"
                            "popfl\n" :
                            "=m"(result) : :
                            "eax",
                            "ecx",
                            "memory"
                        );

                        return(result != 0);
                    #elif defined _WN_64_BIT
                        return(WN_TRUE);
                    #endif
                }

                WN_FORCE_INLINE WN_UINT64 __WNXGetBV(WN_UINT32 _x) {
                    WN_UINT32 result1 = 0;
                    WN_UINT32 result2 = 0;

                    __asm__ __volatile__ (
                        "xgetbv" :
                        // Output
                        "=a" (result1),
                        "=d" (result2) :
                        // Input
                        "c" (_x)
                    );

                    return((static_cast<WN_UINT64>(result2) << 32) | result1);
                }

                WN_FORCE_INLINE WN_VOID __WNCPUID(WN_INT32 _a[4], WN_INT32 _b) {
                    __asm__ __volatile__ (
                        "cpuid" :
                        // Output
                        "=a" (_a[0]),
                        "=b" (_a[1]),
                        "=c" (_a[2]),
                        "=d" (_a[3]) :
                        // Input
                        "a" (_b)
                    );
                }
            #endif
        #endif

        enum __WNSystemEndianness {
            eWNUnknown,
            eWNBig,
            eWNLittle
        };

        enum __WNSystemFeature {
            eWNFPU             = 1,

            #ifdef _WN_X86
                eWNVEXPrefix = (1 << 1),
                eWNCMPXCHG8  = (1 << 2),
                eWNCMPXCHG16 = (1 << 3),
                eWNMMX       = (1 << 4),
                eWNEMMI      = (1 << 5),
                eWN3DNow     = (1 << 6),
                eWN3DNowPlus = (1 << 7),
                eWNSSE       = (1 << 8),
                eWNSSE2      = (1 << 9),
                eWNSSE3      = (1 << 10),
                eWNSSSE3     = (1 << 11),
                eWNSSE41     = (1 << 12),
                eWNSSE42     = (1 << 13),
                eWNSSE4a     = (1 << 14),
                eWNAES       = (1 << 15),
                eWNCLMUL     = (1 << 16),
                eWNFMA3      = (1 << 17),
                eWNFMA4      = (1 << 18),
                eWNF16C      = (1 << 19),
                eWNXOP       = (1 << 20),
                eWNAVX       = (1 << 21),
                eWNAVX2      = (1 << 22)
            #elif defined _WN_ARM
                eWNNeon      = (1 << 1)
            #endif
        };

        class __WNSystemData {
        public:
            WN_FORCE_INLINE __WNSystemData();

            WN_INLINE WN_VOID PopulateValues();

            WN_FORCE_INLINE WN_UINT64 GetTotalPhysicalMemory() const;
            WN_FORCE_INLINE WN_UINT64 GetTotalVirtualMemory() const;

            WN_FORCE_INLINE __WNSystemEndianness GetProcessorEndianness() const;

            WN_FORCE_INLINE WN_UINT64 GetProcessorFeatures() const;
            WN_FORCE_INLINE WN_UINT8 GetProcessorPhysicalCoreCount() const;
            WN_FORCE_INLINE WN_UINT8 GetProcessorLogicalThreadCount() const;

            WN_FORCE_INLINE WN_BOOL IsProcessorHyperThreaded() const;

        private:
            WN_UINT64 mTotalPhysicalMemory;
            WN_UINT64 mTotalVirtualMemory;

            __WNSystemEndianness mProcessorEndianness;

            WN_UINT64 mProcessorFeatures;
            WN_UINT8 mProcessorPhysicalCoreCount;
            WN_UINT8 mProcessorLogicalThreadCount;

            WN_BOOL mProcessorIsHyperThreaded;

            WN_BOOL mAlreadyPopulated;
        };

        WN_FORCE_INLINE __WNSystemData::__WNSystemData() {
            PopulateValues();
        }

        WN_INLINE WN_VOID __WNSystemData::PopulateValues() {
            if (!mAlreadyPopulated) {
                mTotalPhysicalMemory = 0;
                mTotalVirtualMemory = 0;
                mProcessorFeatures = 0;
                mProcessorPhysicalCoreCount = 0;
                mProcessorLogicalThreadCount = 0;
                mProcessorEndianness = eWNUnknown;
                mProcessorIsHyperThreaded = WN_FALSE;

                // Determine processor endianness

                #ifdef _WN_ENDIAN_LITTLE
                    mProcessorEndianness = eWNLittle;
                #elif defined _WN_ENDIAN_BIG
                    mProcessorEndianness = eWNBig;
                #else
                    const WN_UINT32 check = 1;

                    if (*reinterpret_cast<const WN_BYTE*>(&check) == 1) {
                        mProcessorEndianness = eWNLittle;
                    } else {
                        mProcessorEndianness = eWNBig;
                    }
                #endif

                // Determine processor physical core count and logical thread count

                #ifdef _WN_WINDOWS
                    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = WN_NULL;

                    DWORD returnLength = 0;

                    for (;;) {
                        const DWORD returnCode = GetLogicalProcessorInformation(buffer, &returnLength);

                        if (returnCode == 0) {
                            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                                if (buffer != WN_NULL) {
                                    free(buffer);
                                }

                                buffer = static_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(malloc(returnLength));

                                if (buffer == WN_NULL) {
                                    break;
                                }
                            } else {
                                break;
                            }
                        } else {
                            break;
                        }
                    }

                    if (buffer != WN_NULL) {
                        mProcessorPhysicalCoreCount = 0;
                        mProcessorLogicalThreadCount = 0;

                        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = buffer;

                        WN_UINT32 byteOffset = 0;

                        while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) {
                            if (ptr->Relationship == RelationProcessorCore) {
                                mProcessorPhysicalCoreCount++;

                                const WN_SIZE_T shift = sizeof(WN_SIZE_T) * 8 - 1;
                                WN_SIZE_T bitTest = static_cast<WN_SIZE_T>(1) << shift;
                                WN_UINT8 bitSetCount = 0;

                                for (WN_SIZE_T i = 0; i <= shift; ++i) {
                                    if ((ptr->ProcessorMask & bitTest) != 0) {
                                        bitSetCount++;
                                    }

                                    bitTest >>= 1;
                                }

                                mProcessorLogicalThreadCount += bitSetCount;
                            }

                            byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
                            ptr++;
                        }

                        free(buffer);
                    }
                #elif defined _WN_LINUX || defined _WN_ANDROID
                    // parse /proc/cpuinfo based on architecture

                    mProcessorLogicalThreadCount = static_cast<WN_UINT8>(sysconf(_SC_NPROCESSORS_ONLN));
                    mProcessorPhysicalCoreCount = mProcessorLogicalThreadCount >> 2;
                #endif

                if (mProcessorPhysicalCoreCount < 1) {
                    mProcessorPhysicalCoreCount = 1;
                }

                if (mProcessorLogicalThreadCount < 1) {
                    mProcessorLogicalThreadCount = 1;
                }

                // Determine if processor is hyper-threaded

                #ifdef _WN_X86
                    if (__WNInternal::__WNHasCPUID()) {
                        WN_INT32 info[4] = {0};

                        __WNInternal::__WNCPUID(info, 1);

                        if (WN_BIT_TEST(info[3], 28)) {
                            if (mProcessorLogicalThreadCount > mProcessorPhysicalCoreCount) {
                                mProcessorIsHyperThreaded = WN_TRUE;
                            }
                        }
                    }
                #else
                    if (mProcessorLogicalThreadCount > mProcessorPhysicalCoreCount) {
                        mProcessorIsHyperThreaded = WN_TRUE;
                    }
                #endif

                // Determine processor features (i.e. SSE, AVX, NEON)

                #ifdef _WN_ARM
                    #if defined _WN_POSIX
                        FILE *info = fopen ("/proc/cpuinfo", "rt");

                        if (info != NULL) {
                            WN_CHAR* line = WN_NULL;
                            WN_SIZE_T linelen = 0;

                            while (getline(&line, &linelen, info) != -1) {
                                WN_CHAR* cap;

                                if (strncmp(line, "Features\t:", 10)) {
                                    continue;
                                }

                                cap = strstr(line + 10, " neon");

                                if (cap != NULL && (cap[5] == '\0' || cap[5] == ' ')) {
                                    mProcessorFeatures |= eWNNeon;
                                }

                                break;
                            }

                            fclose(info);

                            if (line != WN_NULL) {
                                free(line);
                            }
                        }
                    #endif
                #elif defined _WN_X86
                    if (__WNInternal::__WNHasCPUID()) {
                        WN_INT32 info[4] = {0};

                        __WNInternal::__WNCPUID(info, 1);

                        #ifdef __WN_FPU_AVAILABLE
                            mProcessorFeatures |= eWNFPU;
                        #else
                            mProcessorFeatures |= WN_BIT_TEST(info[3], 0) ? eWNFPU : 0;
                        #endif

                        #ifdef __WN_MMX_AVAILABLE
                            mProcessorFeatures |= eWNMMX;
                        #else
                            mProcessorFeatures |= WN_BIT_TEST(info[3], 23) ? eWNMMX : 0;
                        #endif

                        #ifdef __WN_SSE_AVAILABLE
                            mProcessorFeatures |= eWNSSE;
                        #else
                            mProcessorFeatures |= WN_BIT_TEST(info[3], 25) ? eWNSSE : 0;
                        #endif

                        #ifdef __WN_SSE2_AVAILABLE
                            mProcessorFeatures |= eWNSSE2;
                        #else
                            mProcessorFeatures |= WN_BIT_TEST(info[3], 26) ? eWNSSE2 : 0;
                        #endif

                        #ifdef __WN_SSE3_AVAILABLE
                            mProcessorFeatures |= eWNSSE3;
                        #else
                            mProcessorFeatures |= WN_BIT_TEST(info[2], 0) ? eWNSSE3 : 0;
                        #endif

                        mProcessorFeatures |= WN_BIT_TEST(info[2], 9) ? eWNSSSE3 : 0;
                        mProcessorFeatures |= WN_BIT_TEST(info[2], 19) ? eWNSSE41 : 0;
                        mProcessorFeatures |= WN_BIT_TEST(info[2], 20) ? eWNSSE42 : 0;

                        mProcessorFeatures |= WN_BIT_TEST(info[3], 8) ? eWNCMPXCHG8: 0;
                        mProcessorFeatures |= WN_BIT_TEST(info[2], 13) ? eWNCMPXCHG16: 0;
                        mProcessorFeatures |= WN_BIT_TEST(info[2], 25) ? eWNAES : 0;
                        mProcessorFeatures |= WN_BIT_TEST(info[2], 1) ? eWNCLMUL : 0;
                        mProcessorFeatures |= WN_BIT_TEST(info[2], 29) ? eWNF16C : 0;

                        const WN_BOOL OSXSAVESupported = WN_BIT_TEST(info[2], 27);
                        WN_BOOL OSXStateEnabled = WN_FALSE;

                        if (OSXSAVESupported) {
                            #ifdef _WN_WINDOWS
                                const HMODULE handle = GetModuleHandle("kernel32");
                                const FARPROC address = GetProcAddress(handle, "GetEnabledXStateFeatures");
                                const DWORD64 (WINAPI *GEXSF)() = reinterpret_cast<const DWORD64 (WINAPI *)()>(address);

                                if (GEXSF != WN_NULL) {
                                    const DWORD64 featureMask = GEXSF();

                                    if ((featureMask & __WN_XSTATE_MASK_GSSE) != 0) {
                                        OSXStateEnabled = WN_TRUE;
                                    }
                                }
                            #else
                                const WN_UINT64 XCRFeatureMask = __WNInternal::__WNXGetBV(__WN_XCR_XFEATURE_ENABLED_MASK);

                                if ((XCRFeatureMask & 0x6) == 1) {
                                    OSXStateEnabled = WN_TRUE;
                                }
                            #endif

                            if (OSXStateEnabled) {
                                mProcessorFeatures |= eWNVEXPrefix;
                                mProcessorFeatures |= WN_BIT_TEST(info[2], 12) ? eWNFMA3 : 0;
                                mProcessorFeatures |= WN_BIT_TEST(info[2], 28) ? eWNAVX : 0;

                                __WNInternal::__WNCPUID(info, 3);

                                mProcessorFeatures |= WN_BIT_TEST(info[1], 5) ? eWNAVX2 : 0;
                            }
                        }

                        __WNInternal::__WNCPUID(info, 0x80000000);

                        if (*reinterpret_cast<WN_UINT32*>(&info[0]) >= 0x80000001) {
                            __WNInternal::__WNCPUID(info, 0x80000001);

                            mProcessorFeatures |= WN_BIT_TEST(info[3], 23) ? eWNEMMI : 0;
                            mProcessorFeatures |= WN_BIT_TEST(info[3], 31) ? eWN3DNow : 0;
                            mProcessorFeatures |= WN_BIT_TEST(info[3], 30) ? eWN3DNowPlus : 0;
                            mProcessorFeatures |= WN_BIT_TEST(info[2], 6) ? eWNSSE4a : 0;

                            if (OSXSAVESupported && OSXStateEnabled) {
                                mProcessorFeatures |= WN_BIT_TEST(info[2], 11) ? eWNXOP : 0;
                                mProcessorFeatures |= WN_BIT_TEST(info[2], 16) ? eWNFMA4 : 0;
                            }
                        }
                    }
                #endif

                // Determine total phyical and virtual memory

                #ifdef _WN_WINDOWS
                    MEMORYSTATUSEX memoryInfo;

                    memoryInfo.dwLength = sizeof(MEMORYSTATUSEX);

                    GlobalMemoryStatusEx(&memoryInfo);

                    mTotalPhysicalMemory = memoryInfo.ullTotalPhys;
                    mTotalVirtualMemory = memoryInfo.ullTotalPageFile;
                #elif defined _WN_LINUX
                    struct sysinfo memoryInfo;

                    if (sysinfo(&memoryInfo) == 0) {
                        mTotalPhysicalMemory = memoryInfo.totalram;
                        mTotalPhysicalMemory *= memoryInfo.mem_unit;

                        mTotalVirtualMemory = memoryInfo.totalram;
                        mTotalVirtualMemory += memoryInfo.totalswap;
                        mTotalVirtualMemory *= memoryInfo.mem_unit;
                    }
                #endif

                mAlreadyPopulated = WN_TRUE;
            }
        }

        WN_FORCE_INLINE WN_UINT64 __WNSystemData::GetTotalPhysicalMemory() const {
            return(mTotalPhysicalMemory);
        }

        WN_FORCE_INLINE WN_UINT64 __WNSystemData::GetTotalVirtualMemory() const {
            return(mTotalVirtualMemory);
        }

        WN_FORCE_INLINE WN_UINT64 __WNSystemData::GetProcessorFeatures() const {
            return(mProcessorFeatures);
        }

        WN_FORCE_INLINE WN_UINT8 __WNSystemData::GetProcessorPhysicalCoreCount() const {
            return(mProcessorPhysicalCoreCount);
        }

        WN_FORCE_INLINE WN_UINT8 __WNSystemData::GetProcessorLogicalThreadCount() const {
            return(mProcessorLogicalThreadCount);
        }

        WN_FORCE_INLINE __WNSystemEndianness __WNSystemData::GetProcessorEndianness() const {
            return(mProcessorEndianness);
        }

        WN_FORCE_INLINE WN_BOOL __WNSystemData::IsProcessorHyperThreaded() const {
            return(mProcessorIsHyperThreaded);
        }

        template <typename Type>
        class __WNSystemDataGlobal {
        public:
            static Type mSystemData;
        };

        template <typename Type>
        Type __WNSystemDataGlobal<Type>::mSystemData;
    }

    namespace WNSystem {
        WN_INLINE WN_VOID WNPopulateValues() {
            __WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.PopulateValues();
        }

        WN_FORCE_INLINE WN_UINT8 WNGetPhysicalCoreCount() {
            return(__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorPhysicalCoreCount());
        }

        WN_FORCE_INLINE WN_UINT8 WNGetLogicalThreadCount() {
            return(__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorLogicalThreadCount());
        }

        WN_FORCE_INLINE WN_UINT64 WNGetTotalPhysicalMemory() {
            return(__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetTotalPhysicalMemory());
        }

        WN_FORCE_INLINE WN_UINT64 WNGetTotalVirtualMemory() {
            return(__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetTotalVirtualMemory());
        }

        WN_FORCE_INLINE WN_UINT64 WNGetAvailablePhysicalMemory() {
            WN_UINT64 availablePhysical = 0;

            #ifdef _WN_WINDOWS
                MEMORYSTATUSEX memoryInfo;

                memoryInfo.dwLength = sizeof(MEMORYSTATUSEX);

                GlobalMemoryStatusEx(&memoryInfo);

                availablePhysical = memoryInfo.ullAvailPhys;
            #elif defined _WN_LINUX
                struct sysinfo memoryInfo;

                if (sysinfo(&memoryInfo) == 0) {
                    availablePhysical = memoryInfo.freeram;
                    availablePhysical *= memoryInfo.mem_unit;
                }
            #endif

            return(availablePhysical);
        }

        WN_FORCE_INLINE WN_UINT64 WNGetAvailableVirtualMemory() {
            WN_UINT64 availableVirtual = 0;

            #ifdef _WN_WINDOWS
                MEMORYSTATUSEX memoryInfo;

                memoryInfo.dwLength = sizeof(MEMORYSTATUSEX);

                GlobalMemoryStatusEx(&memoryInfo);

                availableVirtual = memoryInfo.ullAvailPageFile;
            #elif defined _WN_LINUX
                struct sysinfo memoryInfo;

                if (sysinfo(&memoryInfo) == 0) {
                    availableVirtual = memoryInfo.freeram;
                    availableVirtual += memoryInfo.freeswap;
                    availableVirtual *= memoryInfo.mem_unit;
                }
            #endif

            return(availableVirtual);
        }

        WN_FORCE_INLINE WN_BOOL WNIsHyperThreaded() {
            return(__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.IsProcessorHyperThreaded());
        }

        WN_FORCE_INLINE WN_BOOL WNIsLittleEndian() {
            return(__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorEndianness() == __WNInternal::eWNLittle);
        }

        WN_FORCE_INLINE WN_BOOL WNIsBigEndian() {
            return(__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorEndianness() == __WNInternal::eWNBig);
        }

        WN_FORCE_INLINE WN_BOOL WNHasFPU() {
            #ifdef __WN_FPU_AVAILABLE
                return(WN_TRUE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNFPU) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasCMPXCHG8() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNCMPXCHG8) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasCMPXCHG16() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNCMPXCHG16) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasMMX() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNMMX) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasEMMI() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNEMMI) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHas3DNow() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWN3DNow) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHas3DNowPlus() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWN3DNowPlus) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasSSE() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                #ifdef __WN_SSE_AVAILABLE
                    return(WN_TRUE);
                #else
                    return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNSSE) != 0);
                #endif
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasSSE2() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                #ifdef __WN_SSE2_AVAILABLE
                    return(WN_TRUE);
                #else
                    return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNSSE2) != 0);
                #endif
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasSSE3() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                #ifdef __WN_SSE3_AVAILABLE
                    return(WN_TRUE);
                #else
                    return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNSSE3) != 0);
                #endif
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasSSSE3() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNSSSE3) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasSSE41() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNSSE41) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasSSE42() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNSSE42) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasSSE4a() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNSSE4a) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasF16C() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNF16C) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasVEXPrefix() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNVEXPrefix) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasAES() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNAES) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasCLMUL() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNCLMUL) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasFMA3() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNFMA3) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasFMA4() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNFMA4) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasXOP() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNXOP) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasAVX() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNAVX) != 0);
            #endif
        }

        WN_FORCE_INLINE WN_BOOL WNHasAVX2() {
            #ifndef _WN_X86
                return(WN_FALSE);
            #else
                return((__WNInternal::__WNSystemDataGlobal<__WNInternal::__WNSystemData>::mSystemData.GetProcessorFeatures() & __WNInternal::eWNAVX2) != 0);
            #endif
        }
    }
}

#endif // __WN_PLATFORM_INTERNAL_SYSTEM_INL__
