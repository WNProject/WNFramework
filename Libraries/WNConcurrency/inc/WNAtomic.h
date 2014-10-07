////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_ATOMIC_H__
#define __WN_CONCURRENCY_ATOMIC_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"

namespace WNConcurrency {
    WN_ATOM_T WNAtomicCompareSwap(volatile WN_ATOM_T* _destination, WN_ATOM_T _swap, WN_ATOM_T _compare);
    WN_VOID* WNAtomicCompareSwapPointer(WN_VOID* volatile* _destination, WN_VOID* _swap, WN_VOID* _compare);

    WN_BOOL WNAtomicDoubleCompareSwap(volatile WN_ATOM_T* _destination, WN_ATOM_T* _swap, WN_ATOM_T* _compare);
    WN_BOOL WNAtomicDoubleCompareSwapPointer(WN_VOID* volatile* _destination, WN_VOID* _swap, WN_VOID* _compare);

    WN_ATOM_T WNAtomicSwap(volatile WN_ATOM_T* _target, WN_ATOM_T _value);
    WN_VOID* WNAtomicSwapPointer(WN_VOID* volatile* _target, WN_VOID* _value);

    WN_ATOM_T WNAtomicIncrement(volatile WN_ATOM_T* _value);
    WN_ATOM_T WNAtomicDecrement(volatile WN_ATOM_T* _value);

    WN_ATOM_T WNAtomicAnd(volatile WN_ATOM_T* _value, WN_ATOM_T _mask);
    WN_ATOM_T WNAtomicOr(volatile WN_ATOM_T* _value, WN_ATOM_T _mask);
    WN_ATOM_T WNAtomicXOr(volatile WN_ATOM_T* _value, WN_ATOM_T _mask);
}

#include "WNConcurrency/inc/Internal/WNAtomic.inl"

#endif // __WN_CONCURRENCY_ATOMIC_H__