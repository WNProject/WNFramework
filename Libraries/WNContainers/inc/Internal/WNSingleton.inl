////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONTAINERS_SINGLETON_INL__
#define __WN_CONTAINERS_SINGLETON_INL__

#ifndef __WN_CONTAINERS_SINGLETON_H__
    #error "WNSingleton.inl should never be included directly. Please include WNSingleton.h instead"
#endif

#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNAllocation.h"
#include "WNConcurrency/inc/WNCallOnce.h"
#include "WNContainers/inc/WNSingleton.h"

namespace WNContainers {
    template <typename Type>
    Type* WNSingleton<Type>::mInstance = WN_NULL;

    template <typename Type>
    WNConcurrency::WNOnceFlag WNSingleton<Type>::mOnce;

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNSingleton<Type>::Initialize() {
        WNConcurrency::WNCallOnce(mOnce, &CreateInstance);
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNSingleton<Type>::Destroy() {
        if (mInstance != WN_NULL) {
            WN_DELETE(mInstance);

            mInstance = WN_NULL;

            mOnce.Reset();
        }
    }

    template <typename Type>
    WN_FORCE_INLINE Type* WNSingleton<Type>::GetInstancePointer() {
        Initialize();

        return(mInstance);
    }

    template <typename Type>
    WN_FORCE_INLINE Type& WNSingleton<Type>::GetInstanceReference() {
        return(*GetInstancePointer());
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNSingleton<Type>::CreateInstance() {
        if (mInstance == WN_NULL) {
            mInstance = WN_NEW(Type);
        }
    }
}

#endif // __WN_CONTAINERS_SINGLETON_INL__
