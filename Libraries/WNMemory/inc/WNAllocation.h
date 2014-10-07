////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MEMORY_ALLOCATION_H__
#define __WN_MEMORY_ALLOCATION_H__
#include <new>
#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"

#ifdef _WN_WINDOWS
    #include <malloc.h>

    #define WN_STACK_ALLOC(_size) _alloca(_size)
#elif defined _WN_LINUX || defined _WN_ANDROID
    #include <alloca.h>

    #define WN_STACK_ALLOC(_size) alloca(_size)
#endif

//#define WN_NEW(_object) new _object
#define WN_PLACEMENT_NEW(_location) new(_location) 
#define WN_NEW new

#ifdef _WN_DEBUG
    #define WN_DELETE(_object) delete _object; _object = WN_NULL
    #define WN_ARRAY_DELETE(_object) delete[] _object; _object = WN_NULL
#else
    #define WN_DELETE(_object) delete _object
    #define WN_ARRAY_DELETE(_object) delete[] _object
#endif

typedef WN_VOID* (*WN_MALLOC_FUNCTION_T)(const WN_SIZE_T);
typedef WN_VOID* (*WN_CALLOC_FUNCTION_T)(const WN_SIZE_T, const WN_SIZE_T);
typedef WN_VOID* (*WN_REALLOC_FUNCTION_T)(WN_VOID*, const WN_SIZE_T);
typedef WN_VOID (*WN_FREE_FUNCTION_T)(WN_VOID*);

typedef WN_VOID* (*WN_ALIGNED_MALLOC_FUNCTION_T)(const WN_SIZE_T, const WN_SIZE_T);
typedef WN_VOID* (*WN_ALIGNED_CALLOC_FUNCTION_T)(const WN_SIZE_T, const WN_SIZE_T, const WN_SIZE_T);
typedef WN_VOID* (*WN_ALIGNED_REALLOC_FUNCTION_T)(WN_VOID*, const WN_SIZE_T, const WN_SIZE_T);
typedef WN_VOID (*WN_ALIGNED_FREE_FUNCTION_T)(WN_VOID*);

namespace WNMemory {
    WN_VOID* WNMalloc(const WN_SIZE_T _size);
    WN_VOID* WNCalloc(const WN_SIZE_T _count, const WN_SIZE_T _size);
    WN_VOID* WNRealloc(WN_VOID* _memory, const WN_SIZE_T _size);
    WN_VOID WNFree(WN_VOID* _memory);

    template <typename Type>
    Type* WNMallocT(const WN_SIZE_T _amount = 1);

    template <typename Type>
    Type* WNCallocT(const WN_SIZE_T _count, const WN_SIZE_T _amount = 1);

    template <typename Type>
    Type* WNReallocT(Type* _memory, const WN_SIZE_T _amount = 1);

    template <typename Type>
    WN_VOID WNFreeT(Type* _memory);

    WN_VOID* WNAlignedMalloc(const WN_SIZE_T _size, const WN_SIZE_T _alignment);
    WN_VOID* WNAlignedCalloc(const WN_SIZE_T _count, const WN_SIZE_T _size, const WN_SIZE_T _alignment);
    WN_VOID* WNAlignedRealloc(WN_VOID* _memory, const WN_SIZE_T _size, const WN_SIZE_T _alignment);
    WN_VOID WNAlignedFree(WN_VOID* _memory);

    template <typename Type>
    Type* WNAlignedMallocT(const WN_SIZE_T _amount, const WN_SIZE_T _alignment);

    template <typename Type>
    Type* WNAlignedCallocT(const WN_SIZE_T _count, const WN_SIZE_T _amount, const WN_SIZE_T _alignment);

    template <typename Type>
    Type* WNAlignedReallocT(Type* _memory, const WN_SIZE_T _amount, const WN_SIZE_T _alignment);

    template <typename Type>
    WN_VOID WNAlignedFreeT(Type* _memory);
}

#include "WNMemory/inc/Internal/WNAllocation.inl"

#endif // __WN_MEMORY_ALLOCATION_H__
