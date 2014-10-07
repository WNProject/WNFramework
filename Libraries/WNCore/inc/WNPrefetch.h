////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_PREFETCH_H__
#define __WN_CORE_PREFETCH_H__

#include "WNCore/inc/WNTypes.h"

namespace WNCore {
    template <typename Type>
    WN_VOID WNPrefetch(const Type* _address);

    template <typename Type>
    WN_VOID WNPrefetch(const Type* _address, const WN_SIZE_T _offset);
}

#include "WNCore/inc/Internal/WNPrefetch.inl"

#endif // __WN_CORE_PREFETCH_H__