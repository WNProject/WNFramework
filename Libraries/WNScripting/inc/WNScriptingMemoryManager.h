// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_MEMORY_MANAGER_H__
#define __WN_SCRIPTING_MEMORY_MANAGER_H__

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4244)
    #pragma warning(disable: 4152)
    #pragma warning(disable: 4512)
    #pragma warning(disable: 4245)
    #pragma warning(disable: 4127)
    #pragma warning(disable: 4244)
    #pragma warning(disable: 4100)
    #pragma warning(disable: 4245)
    #pragma warning(disable: 4800)
    #pragma warning(disable: 4267)
#endif

#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

#include "WNScripting/inc/WNScriptingEngine.h"

struct AdditionalFunction {
    const char* mFunctionName;
    void* mFunctionPointer;
};
#ifdef _WN_ANDROID
wn_uint32 WN_INLINE __WN_Sync_Fetch_And_Add_4 (wn_uint32* ptr, wn_uint32 val) {
    wn_uint32 a = *ptr;
    *ptr = *ptr + val;
    return(a);
    //return(__sync_fetch_and_add(ptr, val));
}
wn_uint32 WN_INLINE __WN_Sync_Fetch_And_Sub_4 (wn_uint32* ptr, wn_uint32 val) {
    wn_uint32 a = *ptr;
    *ptr = *ptr - val;
    return(a);
    ///return(__sync_fetch_and_sub(ptr, val));
}
static const AdditionalFunction gAdditionalFunctions[] = {
    { "__sync_fetch_and_add_4", (void*)&__WN_Sync_Fetch_And_Add_4 },
    { "__sync_fetch_and_sub_4", (void*)&__WN_Sync_Fetch_And_Sub_4 }
};
#endif

namespace WNScripting {
    class WNScriptingMemoryManager : public llvm::SectionMemoryManager {
    public:
        WNScriptingMemoryManager(WNScriptingEngine& _engine) :
        mEngine(_engine)
        {
        }
        virtual void *getPointerToNamedFunction(const std::string &Name,
                                            bool ) {
#ifdef _WN_ANDROID
                                                int s = 0;
            int e = sizeof(gAdditionalFunctions) / sizeof(gAdditionalFunctions[0]);
            while(e > s) {
                int m = (s + (e - s) / 2);
                int cmp = strcmp(Name.c_str(), gAdditionalFunctions[m].mFunctionName);
                if(cmp == 0) {
                    return(gAdditionalFunctions[m].mFunctionPointer);
                } else if (cmp > 0) {
                    s = m + 1;
                } else if (cmp < 0) {
                    e = m;
                }
            }
#endif
            void* v = mEngine.GetRegisteredFunction(Name.c_str());
            return(v);
        }
        static void mFree(void* ptr) {
            printf("Freeing: %p\n", ptr);
            free(ptr);
        }
        static void* mMalloc(size_t size) {
            void* v = calloc(size, 1);
            printf("Mallocing: %d -> %p\n", static_cast<wn_int32>(size), v);
            return(v);
        }
        //Not sure why, but in win32/winx64 this gets called and getPointerToNamedFunction does not
        uint64_t getSymbolAddress(const std::string &Name) {
            void* v = mEngine.GetRegisteredFunction(Name.c_str());
            if(!v)
            {
                if("malloc" == Name) {
                    return(reinterpret_cast<uint64_t>(&mMalloc));
                } else if ("free" == Name) {
                    return(reinterpret_cast<uint64_t>(&mFree));
                }
            }
            return(reinterpret_cast<uint64_t>(v));
        }
        private:
        WNScriptingEngine& mEngine;
    };
}
#endif//__WN_SCRIPTING_MEMORY_MANAGER_H__
