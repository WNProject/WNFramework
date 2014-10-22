// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_AUTO_CLEANER_H__
#define __WN_CONTAINERS_AUTO_CLEANER_H__

#include "WNCore/inc/WNTypes.h"

namespace WNContainers {
    template<typename T, WN_VOID (*F)(T&)>
    class WNAutoCleaner {
    public:
        WNAutoCleaner(T& _object);
        ~WNAutoCleaner();

        WN_VOID Detach();

        operator T& () const;

        WN_VOID operator = (const T& _other);

        WN_BOOL operator == (const T& _other) const;

    private:
        WNAutoCleaner(const WNAutoCleaner&);
        WNAutoCleaner& operator = (const WNAutoCleaner&);

        WN_VOID* operator new (WN_SIZE_T);
        WN_VOID operator delete (WN_VOID*);

    private:
        WN_BOOL mAttached;
        T& mObject;
    };
}

#include "WNContainers/inc/Internal/WNAutoCleaner.inl"

#endif // __WN_CONTAINERS_AUTO_CLEANER_H__