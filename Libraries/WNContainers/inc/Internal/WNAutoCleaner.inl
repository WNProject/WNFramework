// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_INTERNAL_AUTO_CLEANER_INL__
#define __WN_CONTAINERS_INTERNAL_AUTO_CLEANER_INL__

namespace WNContainers {
    template<typename T, wn_void (*F)(T&)>
    WN_FORCE_INLINE WNAutoCleaner<T, F>::WNAutoCleaner(T& _object) :
        mObject(_object),
        mAttached(wn_true) {
    }

    template<typename T, wn_void (*F)(T&)>
    WN_FORCE_INLINE WNAutoCleaner<T, F>::~WNAutoCleaner() {
        if (mAttached) {
            F(mObject);
        }
    }

    template<typename T, wn_void (*F)(T&)>
    WN_FORCE_INLINE wn_void WNAutoCleaner<T, F>::Detach() {
        mAttached = wn_false;
    }

    template<typename T, wn_void (*F)(T&)>
    WN_FORCE_INLINE WNAutoCleaner<T, F>::operator T& () const {
        return(mObject);
    }

    template<typename T, wn_void (*F)(T&)>
    WN_FORCE_INLINE wn_void WNAutoCleaner<T, F>::operator = (const T& _other) {
        mObject = _other;
    }

    template<typename T, wn_void (*F)(T&)>
    WN_FORCE_INLINE wn_bool WNAutoCleaner<T, F>::operator == (const T& _other) const {
        return(mObject == _other);
    }
}

#endif // __WN_CONTAINERS_INTERNAL_AUTO_CLEANER_INL__
