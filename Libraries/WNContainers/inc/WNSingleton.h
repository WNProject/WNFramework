// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_SINGLETON_H__
#define __WN_CONTAINERS_SINGLETON_H__

#include "WNCore/inc/WNTypes.h"
#include "WNConcurrency/inc/WNCallOnce.h"

namespace WNContainers {
    template <typename Type>
    class WNSingleton {
    public:
        static WN_VOID Initialize();
        static WN_VOID Destroy();

        static Type* GetInstancePointer();
        static Type& GetInstanceReference();

    protected:
        WNSingleton();
        virtual ~WNSingleton();

    protected:
        static Type* mInstance;

    private:
        WNSingleton(const WNSingleton&);
        WNSingleton& operator = (const WNSingleton&);

        static WN_VOID CreateInstance();

    private:
        static WNConcurrency::WNOnceFlag mOnce;
    };
}

#include "WNContainers/inc/Internal/WNSingleton.inl"

#endif // __WN_CONTAINERS_SINGLETON_H__
