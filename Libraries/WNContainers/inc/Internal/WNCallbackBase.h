// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_INTERNAL_CALLBACK_BASE_H__
#define __WN_CONTAINERS_INTERNAL_CALLBACK_BASE_H__

#ifndef __WN_CONTAINERS_CALLBACK_H__
    #error "Internal/WNCallbackBase.h should never be included directly. Please include WNCallback.h instead"
#endif

#include "WNCore/inc/WNTypes.h"

#include <atomic>

namespace WNContainers {
    namespace internal {
        template <typename Return>
        class __WNCallbackBase0 {
        public:
            __WNCallbackBase0();
            virtual WN_FORCE_INLINE ~__WNCallbackBase0() {}
            virtual Return Execute() const = 0;

            wn_void AddRef();
            wn_size_t RemoveRef();

        private:
            std::atomic<wn_size_t> mCount;
        };

        template <typename Return, typename Parameter>
        class __WNCallbackBase1 {
        public:
            __WNCallbackBase1();
            virtual WN_FORCE_INLINE ~__WNCallbackBase1() {}
            virtual Return Execute(const Parameter& _parameter) const = 0;

            wn_void AddRef();
            wn_size_t RemoveRef();

        private:
            std::atomic<wn_size_t> mCount;
        };

        template<typename Return, typename Parameter1, typename Parameter2>
        class __WNCallbackBase2 {
        public:
            __WNCallbackBase2();
            virtual WN_FORCE_INLINE ~__WNCallbackBase2() {}
            virtual Return Execute(const Parameter1& _parameter1, const Parameter2& _parameter2) const = 0;

            wn_void AddRef();
            wn_size_t RemoveRef();

        private:
            std::atomic<wn_size_t> mCount;
        };

        template<typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
        class __WNCallbackBase3 {
        public:
            __WNCallbackBase3();
            virtual WN_FORCE_INLINE ~__WNCallbackBase3() {}
            virtual Return Execute(const Parameter1& _parameter1, const Parameter2& _parameter2, const Parameter3& _parameter3) const = 0;

            wn_void AddRef();
            wn_size_t RemoveRef();

        private:
            std::atomic<wn_size_t> mCount;
        };

        template <typename Class, typename Return>
        class __WNCallbackInternal0 : public __WNCallbackBase0<Return> {
        public:
            __WNCallbackInternal0(Return (Class::*_function)(), Class* _this);

            Return Execute() const;

        private:
            Class* mObject;
            Return (Class::*mFunction)();
        };

        template <typename Class, typename Return, typename Parameter>
        class __WNCallbackInternal1 : public __WNCallbackBase1<Return, Parameter> {
        public:
            __WNCallbackInternal1(Return (Class::*_function)(Parameter), Class* _this);

            Return Execute(const Parameter& _parameter) const;

        private:
            Class* mObject;
            Return (Class::*mFunction)(Parameter);
        };

        template <typename Class, typename Return, typename Parameter1, typename Parameter2>
        class __WNCallbackInternal2 : public __WNCallbackBase2<Return, Parameter1, Parameter2> {
        public:
            __WNCallbackInternal2(Class* _this, Return (Class::*_function)(Parameter1, Parameter2));

            Return Execute(const Parameter1& _parameter1, const Parameter2& _parameter2) const;

        private:
            Class* mObject;
            Return (Class::*mFunction)(Parameter1, Parameter2);
        };

        template <typename Class, typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
        class __WNCallbackInternal3 : public __WNCallbackBase3<Return, Parameter1, Parameter2, Parameter3> {
        public:
            __WNCallbackInternal3(Class* _this, Return (Class::*_function)(Parameter1, Parameter2, Parameter3));

            Return Execute(const Parameter1& _parameter1, const Parameter2& _parameter2, const Parameter3& _parameter3) const;

        private:
            Class* mObject;
            Return (Class::*mFunction)(Parameter1, Parameter2, Parameter3);
        };

        template <typename Return>
        class __WNCallbackClasslessInternal0 : public __WNCallbackBase0<Return> {
        public:
            __WNCallbackClasslessInternal0(Return (*_function)());

            virtual Return Execute() const;

        private:
            Return (*mFunction)();
        };

        template <typename Return, typename Parameter>
        class __WNCallbackClasslessInternal1 : public __WNCallbackBase1<Return, Parameter> {
        public:
            __WNCallbackClasslessInternal1(Return (*_function)(Parameter));

            virtual Return Execute(const Parameter& _parameter) const;

        private:
            Return (*mFunction)(Parameter);
        };

        template <typename Return, typename Parameter1, typename Parameter2>
        class __WNCallbackClasslessInternal2 : public __WNCallbackBase2<Return, Parameter1, Parameter2> {
        public:
            __WNCallbackClasslessInternal2(Return (*_function)(Parameter1, Parameter2));

            virtual Return Execute(const Parameter1& _parameter1, const Parameter2& _parameter2) const;

        private:
            Return (*mFunction)(Parameter1, Parameter2);
        };

        template <typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
        class __WNCallbackClasslessInternal3 : public __WNCallbackBase3<Return, Parameter1, Parameter2, Parameter3> {
        public:
            __WNCallbackClasslessInternal3(Return (*_function)(Parameter1, Parameter2, Parameter3));

            virtual Return Execute(const Parameter1& _parameter1, const Parameter2& _parameter2, const Parameter3& _parameter3) const;

        private:
            Return (*mFunction)(Parameter1, Parameter2, Parameter3);
        };

    }
}

#include "WNContainers/inc/Internal/WNCallbackBase.inl"

#endif // __WN_CONTAINERS_INTERNAL_CALLBACK_BASE_H__