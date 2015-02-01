// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_INTERNAL_RESOURCE_POINTER_INL__
#define __WN_CONCURRENCY_INTERNAL_RESOURCE_POINTER_INL__

#ifndef __WN_CONCURRENCY_RESOURCE_POINTER_H__
    #error "WNResourcePointer.inl should never be included directly. Please include WNResourcePointer.h instead"
#endif

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNAssert.h"
#include "WNConcurrency/inc/WNAtomic.h"
#include "WNConcurrency/inc/WNResourcePointer.h"
#include "WNMemory/inc/WNAllocation.h"

namespace WNConcurrency {
    template <typename Type>
    WN_FORCE_INLINE WNResourcePointer<Type>::WNResourcePointer() :
        mResource(WN_NULL) {
    }

    template <typename Type>
    WN_FORCE_INLINE WNResourcePointer<Type>::WNResourcePointer(Type* _resource) {
        WN_DEBUG_ASSERT(_resource != WN_NULL);
        WN_DEBUG_ASSERT(_resource->GetRefs() == 0);

        _resource->AddRef();

        mResource = _resource;
    }

    template <typename Type>
    WN_FORCE_INLINE WNResourcePointer<Type>::~WNResourcePointer() {
        if (mResource != WN_NULL) {
            RemoveRef();
        }
    }

    template <typename Type>
    WN_FORCE_INLINE WNResourcePointer<Type>::WNResourcePointer(const WNResourcePointer<Type>& _resourcePointer) {
        if (_resourcePointer.mResource) {
            _resourcePointer.mResource->AddRef();
        }

        mResource = _resourcePointer.mResource;
    }

    template <typename Type>
    template <typename OtherType>
    WN_FORCE_INLINE WNResourcePointer<Type>::WNResourcePointer(const WNResourcePointer<OtherType>& _resourcePointer) {
        if (_resourcePointer.mResource) {
            _resourcePointer.mResource->AddRef();
        }

        mResource = _resourcePointer.mResource;
    }

    template <typename Type>
    WN_FORCE_INLINE Type* WNResourcePointer<Type>::operator -> () {
        return(mResource);
    }

    template <typename Type>
    WN_FORCE_INLINE const Type* WNResourcePointer<Type>::operator -> () const {
        return(mResource);
    }

    template <typename Type>
    WN_FORCE_INLINE Type& WNResourcePointer<Type>::operator * () {
        return(*mResource);
    }

    template <typename Type>
    WN_FORCE_INLINE WNResourcePointer<Type>::operator WN_BOOL () const {
        return(mResource != WN_NULL);
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNResourcePointer<Type>::RemoveRef() {
        if (mResource) {
            if (mResource->RemoveRef()) {
                WN_DELETE(mResource);
            }
        }

        mResource = WN_NULL;
    }

    template <typename Type>
    WN_FORCE_INLINE WNResourcePointer<Type> WNAllocateResource() {
        return(WNResourcePointer<Type>(WN_NEW Type));
    }

    template <typename Type, typename Parameter1>
    WN_FORCE_INLINE WNResourcePointer<Type> WNAllocateResource(Parameter1 _p1) {
        return(WNResourcePointer<Type>(WN_NEW Type(_p1)));
    }

    template <typename Type, typename Parameter1, typename Parameter2>
    WN_FORCE_INLINE WNResourcePointer<Type> WNAllocateResource(Parameter1 _p1, Parameter2 _p2) {
        return(WNResourcePointer<Type>(WN_NEW Type(_p1, _p2)));
    }

    template <typename Type, typename Parameter1, typename Parameter2, typename Parameter3>
    WN_FORCE_INLINE WNResourcePointer<Type> WNAllocateResource(Parameter1 _p1, Parameter2 _p2, Parameter3 _p3) {
        return(WNResourcePointer<Type>(WN_NEW Type(_p1, _p2, _p3)));
    }

    template <typename Type, typename Parameter1, typename Parameter2, typename Parameter3, typename Parameter4>
    WN_FORCE_INLINE WNResourcePointer<Type> WNAllocateResource(Parameter1 _p1, Parameter2 _p2, Parameter3 _p3, Parameter4 _p4) {
        return(WNResourcePointer<Type>(WN_NEW Type(_p1, _p2, _p3, _p4)));
    }
    
    template<typename Type>
    WN_FORCE_INLINE WNResourcePointer<Type>& WNResourcePointer<Type>::operator = (const WNResourcePointer<Type>& _rp) {
        if (_rp.mResource) {
            _rp.mResource->AddRef();
        }

        RemoveRef();

        mResource = _rp.mResource;

        return(*this);
    }

    template<typename Type>
    template<typename OtherType>
    WN_FORCE_INLINE WNResourcePointer<Type>& WNResourcePointer<Type>::operator = (const WNResourcePointer<OtherType>& _rp) {
        if (_rp.mResource) {
            _rp.mResource->AddRef();
        }

        RemoveRef();

        mResource = _rp.mResource;

        return(*this);
    }

    template <typename Type, typename Type2>
    WN_FORCE_INLINE WN_BOOL operator == (const WNResourcePointer<Type>& _rp1, const WNResourcePointer<Type2>& _rp2) {
        return(_rp1.mResource == _rp2.mResource);
    }
}

#endif // __WN_CONCURRENCY_INTERNAL_RESOURCE_POINTER_INL__
