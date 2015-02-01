// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_RESOURCE_POINTER_H__
#define __WN_CONCURRENCY_RESOURCE_POINTER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNAssert.h"
#include "WNConcurrency/inc/WNAtomic.h"
#include "WNMemory/inc/WNAllocation.h"

namespace WNConcurrency {
    template <typename Type>
    class WNResourcePointer {
    public:
        WNResourcePointer();
        WNResourcePointer(Type* _resource);
        
        WNResourcePointer(const WNResourcePointer& _resourcePointer);

        template <typename OtherType>
        WNResourcePointer(const WNResourcePointer<OtherType>& _resourcePointer);

        ~WNResourcePointer();

        Type* operator -> ();
        const Type* operator -> () const;

        Type& operator * ();

        operator WN_BOOL () const;

        WNResourcePointer& operator = (const WNResourcePointer& _resourcePointer);
        
        template <typename OtherType>
        WNResourcePointer& operator = (const WNResourcePointer<OtherType>& _resourcePointer);

    private:
        template <typename OtherType>
        friend class WNResourcePointer;

    private:
        WN_VOID RemoveRef();

    private:
        Type* mResource;
    };
    
    template <typename Type, typename Type2>
    WN_BOOL operator == (const WNResourcePointer<Type>& _t1, const WNResourcePointer<Type2>& _t2);

    template <typename Type>
    WNResourcePointer<Type> WNAllocateResource();

    template <typename Type, typename Parameter1>
    WNResourcePointer<Type> WNAllocateResource(Parameter1 _p1);

    template <typename Type, typename Parameter1, typename Parameter2>
    WNResourcePointer<Type> WNAllocateResource(Parameter1 _p1, Parameter2 _p2);

    template <typename Type, typename Parameter1, typename Parameter2, typename Parameter3>
    WNResourcePointer<Type> WNAllocateResource(Parameter1 _p1, Parameter2 _p2, Parameter3 _p3);

    template <typename Type, typename Parameter1, typename Parameter2, typename Parameter3, typename Parameter4>
    WNResourcePointer<Type> WNAllocateResource(Parameter1 _p1, Parameter2 _p2, Parameter3 _p3, Parameter4 _p4);

}

#include "WNConcurrency/inc/Internal/WNResourcePointer.inl"

#endif // __WN_CONCURRENCY_RESOURCE_POINTER_H__