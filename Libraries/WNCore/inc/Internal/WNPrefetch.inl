////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_INTERNAL_PREFETCH_INL__
#define __WN_CORE_INTERNAL_PREFETCH_INL__

#include "WNCore/inc/Internal/WNPrefetch.h"

namespace WNCore {
    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNPrefetch(const Type* _address) {
        __WNInternal::__WNPrefetch<Type>::Execute(_address);
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNPrefetch(const Type* _address, const WN_SIZE_T _offset) {
        __WNInternal::__WNPrefetch<Type>::Execute(reinterpret_cast<const WN_BYTE*>(_address) + _offset);
    }
}

#endif // __WN_CORE_INTERNAL_PREFETCH_INL__