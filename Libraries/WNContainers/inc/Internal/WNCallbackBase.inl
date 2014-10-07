////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONTAINERS_INTERNAL_CALLBACK_BASE_INL__
#define __WN_CONTAINERS_INTERNAL_CALLBACK_BASE_INL__

#ifndef __WN_CONTAINERS_CALLBACK_H__
    #error "Internal/WNCallbackBase.inl should never be included directly. Please include WNCallback.h instead"
#endif

#include "WNCore/inc/WNAssert.h"
#include "WNConcurrency/inc/WNAtomic.h"

namespace WNContainers {
    namespace __WNInternal {
        template <typename Class, typename Return>
        WN_FORCE_INLINE __WNCallbackInternal0<Class, Return>::__WNCallbackInternal0(Return (Class::*_function)(), Class* _object) :
            __WNCallbackBase0<Return>(),
            mObject(_object),
            mFunction(_function) {
        }

        template <typename Class, typename Return>
        WN_FORCE_INLINE Return __WNCallbackInternal0<Class, Return>::Execute() const {
            WN_RELEASE_ASSERT_DESC(mObject != WN_NULL, "Object pointer is null");
            WN_RELEASE_ASSERT_DESC(mFunction != WN_NULL, "Invalid function pointer");

            return(mObject->mFunction());
        }

        template <typename Class, typename Return, typename Parameter>
        WN_FORCE_INLINE __WNCallbackInternal1<Class, Return, Parameter>::__WNCallbackInternal1(Return (Class::*_function)(Parameter), Class* _this) :
            __WNCallbackBase1<Return, Parameter>(),
            mObject(_this),
            mFunction(_function) {
        }

        template <typename Class, typename Return, typename Parameter>
        WN_FORCE_INLINE Return __WNCallbackInternal1<Class, Return, Parameter>::Execute(const Parameter& _parameter) const {
            WN_RELEASE_ASSERT_DESC(mObject != WN_NULL, "Object pointer is null");
            WN_RELEASE_ASSERT_DESC(mFunction != WN_NULL, "Invalid function pointer");

            return(mObject->mFunction(_parameter));
        }

        template <typename Class, typename Return, typename Parameter1, typename Parameter2>
        WN_FORCE_INLINE __WNCallbackInternal2<Class, Return, Parameter1, Parameter2>::__WNCallbackInternal2(Class* _this, Return (Class::*_function)(Parameter1, Parameter2)) :
            __WNCallbackBase2<Return, Parameter1, Parameter2>(),
            mObject(_this),
            mFunction(_function) {
        }

        template <typename Class, typename Return, typename Parameter1, typename Parameter2>
        WN_FORCE_INLINE Return __WNCallbackInternal2<Class, Return, Parameter1, Parameter2>::Execute(const Parameter1& _parameter1, const Parameter2& _parameter2) const {
            WN_RELEASE_ASSERT_DESC(mObject != WN_NULL, "Object pointer is null");
            WN_RELEASE_ASSERT_DESC(mFunction != WN_NULL, "Invalid function pointer");

            return(((*mObject).*mFunction)(_parameter1, _parameter2));
        }

        template <typename Class, typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
        WN_FORCE_INLINE __WNCallbackInternal3<Class, Return, Parameter1, Parameter2, Parameter3>::__WNCallbackInternal3(Class* _this, Return (Class::*_function)(Parameter1, Parameter2, Parameter3)) :
            __WNCallbackBase3<Return, Parameter1, Parameter2, Parameter3>(),
            mObject(_this),
            mFunction(_function) {
        }

        template <typename Class, typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
        WN_FORCE_INLINE Return __WNCallbackInternal3<Class, Return, Parameter1, Parameter2, Parameter3>::Execute(const Parameter1& _parameter1, const Parameter2& _parameter2, const Parameter3& _parameter3) const {
            WN_RELEASE_ASSERT_DESC(mObject != WN_NULL, "Object pointer is null");
            WN_RELEASE_ASSERT_DESC(mFunction != WN_NULL, "Invalid function pointer");

            return(((*mObject).*mFunction)(_parameter1, _parameter2, _parameter3));
        }

        template <typename Return>
        WN_FORCE_INLINE __WNCallbackClasslessInternal0<Return>::__WNCallbackClasslessInternal0(Return (*_function)()) :
            __WNCallbackBase0<Return>(),
            mFunction(_function) {
        }

        template <typename Return>
        WN_FORCE_INLINE Return __WNCallbackClasslessInternal0<Return>::Execute() const {
            WN_RELEASE_ASSERT_DESC(mFunction != WN_NULL, "Invalid function pointer");

            return(mFunction());
        }

        template <typename Return, typename Parameter>
        WN_FORCE_INLINE __WNCallbackClasslessInternal1<Return, Parameter>::__WNCallbackClasslessInternal1(Return (*_function)(Parameter)) :
            __WNCallbackBase1<Return, Parameter>(),
            mFunction(_function) {
        }

        template <typename Return, typename Parameter>
        WN_FORCE_INLINE Return __WNCallbackClasslessInternal1<Return, Parameter>::Execute(const Parameter& _parameter) const {
            WN_RELEASE_ASSERT_DESC(mFunction != WN_NULL, "Invalid function pointer");

            return(mFunction(_parameter));
        }

        template <typename Return, typename Parameter1, typename Parameter2>
        WN_FORCE_INLINE __WNCallbackClasslessInternal2<Return, Parameter1, Parameter2>::__WNCallbackClasslessInternal2(Return (*_function)(Parameter1, Parameter2)) :
            __WNCallbackBase2<Return, Parameter1, Parameter2>(),
            mFunction(_function) {
        }

        template <typename Return, typename Parameter1, typename Parameter2>
        Return __WNCallbackClasslessInternal2<Return, Parameter1, Parameter2>::Execute(const Parameter1& _parameter1, const Parameter2& _parameter2) const {
            WN_RELEASE_ASSERT_DESC(mFunction != WN_NULL, "Invalid function pointer");

            return(mFunction(_parameter1, _parameter2));
        }

        template <typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
        WN_FORCE_INLINE __WNCallbackClasslessInternal3<Return, Parameter1, Parameter2, Parameter3>::__WNCallbackClasslessInternal3(Return (*_function)(Parameter1, Parameter2, Parameter3)) :
            __WNCallbackBase3<Return, Parameter1, Parameter2, Parameter3>(),
            mFunction(_function) {
        }

        template <typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
        WN_FORCE_INLINE Return __WNCallbackClasslessInternal3<Return, Parameter1, Parameter2, Parameter3>::Execute(const Parameter1& _parameter1, const Parameter2& _parameter2, const Parameter3& _parameter3) const {
            WN_RELEASE_ASSERT_DESC(mFunction != WN_NULL, "Invalid function pointer");

            return(mFunction(_parameter1, _parameter2, _parameter3));
        }
        
        template <typename Return>
        WN_FORCE_INLINE __WNCallbackBase0<Return>::__WNCallbackBase0() :
            mCount(0) {
        }

        template <typename Return>
        WN_FORCE_INLINE WN_VOID __WNCallbackBase0<Return>::AddRef() {
            WNConcurrency::WNAtomicIncrement(&mCount);
        }

        template <typename Return>
        WN_FORCE_INLINE WN_ATOM_T __WNCallbackBase0<Return>::RemoveRef() {
            return(WNConcurrency::WNAtomicDecrement(&mCount));
        }
        
        template <typename Return, typename Parameter>
        WN_FORCE_INLINE __WNCallbackBase1<Return, Parameter>::__WNCallbackBase1() :
            mCount(0) {
        }

        template <typename Return, typename Parameter>
        WN_FORCE_INLINE WN_VOID __WNCallbackBase1<Return, Parameter>::AddRef() {
            WNConcurrency::WNAtomicIncrement(&mCount);
        }

        template <typename Return, typename Parameter>
        WN_FORCE_INLINE WN_ATOM_T __WNCallbackBase1<Return, Parameter>::RemoveRef() {
            return(WNConcurrency::WNAtomicDecrement(&mCount));
        }

        template <typename Return, typename Parameter1, typename Parameter2>
        WN_FORCE_INLINE __WNCallbackBase2<Return, Parameter1, Parameter2>::__WNCallbackBase2() :
            mCount(0) {
        }

        template <typename Return, typename Parameter1, typename Parameter2>
        WN_FORCE_INLINE WN_VOID __WNCallbackBase2<Return, Parameter1, Parameter2>::AddRef() {
            WNConcurrency::WNAtomicIncrement(&mCount);
        }

        template <typename Return, typename Parameter1, typename Parameter2>
        WN_FORCE_INLINE WN_ATOM_T __WNCallbackBase2<Return, Parameter1, Parameter2>::RemoveRef() {
            return(WNConcurrency::WNAtomicDecrement(&mCount));
        }

        template <typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
        WN_FORCE_INLINE __WNCallbackBase3<Return, Parameter1, Parameter2, Parameter3>::__WNCallbackBase3() :
            mCount(0) {
        }

        template <typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
        WN_FORCE_INLINE WN_VOID __WNCallbackBase3<Return, Parameter1, Parameter2, Parameter3>::AddRef() {
            WNConcurrency::WNAtomicIncrement(&mCount);
        }

        template <typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
        WN_FORCE_INLINE WN_ATOM_T __WNCallbackBase3<Return, Parameter1, Parameter2, Parameter3>::RemoveRef() {
            return(WNConcurrency::WNAtomicDecrement(&mCount));
        }
    }
}

#endif // __WN_CONTAINERS_INTERNAL_CALLBACK_BASE_INL__