////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MEMORY_MANIPULATION_H__
#define __WN_MEMORY_MANIPULATION_H__

#include "WNCore/inc/WNTypes.h"

typedef WN_VOID* (*WN_MEMCLR_FUNCTION_T)(WN_VOID*, const WN_SIZE_T);
typedef WN_VOID* (*WN_MEMSET_FUNCTION_T)(WN_VOID*, const WN_UINT32, const WN_SIZE_T);
typedef WN_VOID* (*WN_MEMCPY_FUNCTION_T)(WN_VOID*, const WN_VOID*, const WN_SIZE_T);
typedef WN_UINT32 (*WN_MEMCMP_FUNCTION_T)(const WN_VOID*, const WN_VOID*, const WN_SIZE_T);
typedef WN_VOID* (*WN_MEMMOVE_FUNCTION_T)(WN_VOID*, const WN_VOID*, const WN_SIZE_T);

namespace WNMemory {
    WN_VOID* WNMemClr(WN_VOID* _memory, const WN_SIZE_T _size);
    WN_VOID* WNMemSet(WN_VOID* _memory, const WN_UINT8 _value, const WN_SIZE_T _size);
    WN_VOID* WNMemCpy(WN_VOID* _destination, const WN_VOID* _source, const WN_SIZE_T _number);
    WN_UINT32 WNMemCmp(const WN_VOID* _memory1, const WN_VOID* _memory2, const WN_SIZE_T _number);
    WN_VOID* WNMemMove(WN_VOID* _destination, const WN_VOID* _source, const WN_SIZE_T _number);

    template <typename Type>
    Type* WNMemClrT(Type* _memory, const WN_SIZE_T _amount = 1);

    // MemSet
    template <typename Type>
    Type* WNMemSetT(Type* _memory, const WN_UINT8 _value, const WN_SIZE_T _amount = 1);
    
    template <>
    WN_VOID* WNMemSetT(WN_VOID* _memory, WN_UINT8 _value, const WN_SIZE_T _amount);

    // MemCpy
    template <typename Type>
    Type* WNMemCpyT(Type* _destination, const Type* _source, const WN_SIZE_T _number = 1);

    template <typename Type>
    WN_UINT32 WNMemCmpT(const Type* _memory1, const Type* _memory2, const WN_SIZE_T _number = 1);

    template <typename Type>
    Type* WNMemMoveT(Type* _destination, const Type* _source, const WN_SIZE_T _number = 1);
}

#include "WNMemory/inc/Internal/WNManipulation.inl"

#endif // __WN_MEMORY_MANIPULATION_H__