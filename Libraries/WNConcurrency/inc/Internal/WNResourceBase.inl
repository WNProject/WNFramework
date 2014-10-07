////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_INTERNAL_RESOURCE_BASE_INL__
#define __WN_CONCURRENCY_INTERNAL_RESOURCE_BASE_INL__

#ifndef __WN_CONCURRENCY_RESOURCE_BASE_H__
    #error "WNResourceBase.inl should never be included directly. Please include WNResourceBase.h instead"
#endif

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNAssert.h"
#include "WNConcurrency/inc/WNAtomic.h"
#include "WNConcurrency/inc/WNResourceBase.h"

namespace WNConcurrency {
    WN_FORCE_INLINE WNResourceBase::WNResourceBase() :
        mRefCount(0) {
    }

    WN_FORCE_INLINE WN_VOID WNResourceBase::AddRef() {
        WN_ATOM_T targetCount;
        WN_ATOM_T oldCount;

        do {
            targetCount = mRefCount + 1;
            oldCount = WNAtomicCompareSwap(&mRefCount, targetCount, targetCount - 1);
        } while (oldCount != (targetCount - 1));
    }

    WN_FORCE_INLINE WN_BOOL WNResourceBase::RemoveRef() {
        WN_ATOM_T targetCount;
        WN_ATOM_T oldCount;

        do {
            WN_DEBUG_ASSERT(mRefCount > 0);

            targetCount = mRefCount - 1;
            oldCount = WNAtomicCompareSwap(&mRefCount, targetCount, targetCount + 1);
        } while (oldCount != (targetCount + 1));

        if (targetCount == 0) {
            return(WN_TRUE);
        }

        return(WN_FALSE);
    }

    WN_FORCE_INLINE WN_ATOM_T WNResourceBase::GetRefs() {
        return(mRefCount);
    }
}

#endif // __WN_CONCURRENCY_INTERNAL_RESOURCE_BASE_INL__