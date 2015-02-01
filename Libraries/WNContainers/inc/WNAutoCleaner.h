// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_AUTO_CLEANER_H__
#define __WN_CONTAINERS_AUTO_CLEANER_H__

#include "WNCore/inc/WNTypes.h"

namespace WNContainers {
    template<typename T, wn_void (*F)(T&)>
    class WNAutoCleaner {
    public:
        WNAutoCleaner(T& _object);
        ~WNAutoCleaner();

        wn_void Detach();

        operator T& () const;

        wn_void operator = (const T& _other);

        wn_bool operator == (const T& _other) const;

    private:
        WNAutoCleaner(const WNAutoCleaner&);
        WNAutoCleaner& operator = (const WNAutoCleaner&);

        wn_void* operator new (wn_size_t);
        wn_void operator delete (wn_void*);

    private:
        wn_bool mAttached;
        T& mObject;
    };
}

#include "WNContainers/inc/Internal/WNAutoCleaner.inl"

#endif // __WN_CONTAINERS_AUTO_CLEANER_H__