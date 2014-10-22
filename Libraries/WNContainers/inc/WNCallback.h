// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_CALLBACK_H__
#define __WN_CONTAINERS_CALLBACK_H__

#include "WNContainers/inc/Internal/WNCallbackBase.h"

namespace WNContainers {
    template <typename Return>
    class WNCallback0 {
    public:
        WNCallback0();
        WNCallback0(const WNCallback0<Return>& _callback);
        WNCallback0(Return (*_function)());

        template <typename Class>
        WNCallback0(Class* _this, Return (Class::*_function)());

        virtual ~WNCallback0();

        WNCallback0<Return>& operator = (const WNCallback0<Return>& _callback);

        Return Execute() const;
        WN_BOOL IsValid();
    private:
        __WNInternal::__WNCallbackBase0<Return>* mBaseCallback;
    };

    template <typename Return, typename Parameter>
    class WNCallback1 {
    public:
        WNCallback1();
        WNCallback1(const WNCallback1<Return, Parameter>& _callback);
        WNCallback1(Return (*_function)(Parameter));

        template <typename Class>
        WNCallback1(Class* _this, Return (Class::*_function)(Parameter));

        virtual ~WNCallback1();

        WNCallback1<Return, Parameter>& operator = (const WNCallback1<Return, Parameter>& _callback);

        Return Execute(const Parameter& _parameter) const;
        WN_BOOL IsValid();
    private:
        __WNInternal::__WNCallbackBase1<Return, Parameter>* mBaseCallback;
    };

    template <typename Return, typename Parameter1, typename Parameter2>
    class WNCallback2 {
    public:
        WNCallback2();
        WNCallback2(const WNCallback2<Return, Parameter1, Parameter2>& _callback);
        WNCallback2(Return (*_function)(Parameter1, Parameter2));

        template<typename Class>
        WNCallback2(Class* _this, Return (Class::*_function)(Parameter1, Parameter2));

        virtual ~WNCallback2();

        WNCallback2<Return, Parameter1, Parameter2>& operator = (const WNCallback2<Return, Parameter1, Parameter2>& _callback);

        Return Execute(const Parameter1& _parameter1, const Parameter2& _parameter2) const;
        WN_BOOL IsValid();
    private:
        __WNInternal::__WNCallbackBase2<Return, Parameter1, Parameter2>* mBaseCallback;
    };

    template <typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
    class WNCallback3 {
    public:
        WNCallback3();
        WNCallback3(const WNCallback3<Return, Parameter1, Parameter2, Parameter3>& _callback);
        WNCallback3(Return (*_function)(Parameter1, Parameter2, Parameter3));
        template<typename Class>
        WNCallback3(Class* _this, Return (Class::*_function)(Parameter1, Parameter2, Parameter3));

        virtual ~WNCallback3();

        WNCallback3<Return, Parameter1, Parameter2, Parameter3>& operator = (const WNCallback3<Return, Parameter1, Parameter2, Parameter3>& _callback);

        Return Execute(const Parameter1& _parameter1, const Parameter2& _parameter2, const Parameter3& _parameter3) const;
        WN_BOOL IsValid();
    private:
        __WNInternal::__WNCallbackBase3<Return, Parameter1, Parameter2, Parameter3>* mBaseCallback;
    };
}

#include "WNContainers/inc/Internal/WNCallback.inl"

#endif // __WN_CONTAINERS_CALLBACK_H__
