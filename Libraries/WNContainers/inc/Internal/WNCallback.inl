////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONTAINERS_CALLBACK_INL__
#define __WN_CONTAINERS_CALLBACK_INL__

#ifndef __WN_CONTAINERS_CALLBACK_H__
    #error "WNCallback.inl should never be included directly. Please include WNCallback.h instead"
#endif

#include "WNMemory/inc/WNAllocation.h"

namespace WNContainers {
    template <typename Return>
    WN_FORCE_INLINE WNCallback0<Return>::WNCallback0() :
        mBaseCallback(WN_NULL) {
    }

    template <typename Return>
    WN_FORCE_INLINE WNCallback0<Return>::WNCallback0(const WNCallback0<Return>& _callback) :
        mBaseCallback(_callback.mBaseCallback) {
        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }
    }

    template <typename Return>
    WN_FORCE_INLINE WNCallback0<Return>::WNCallback0(Return (*_function)()) {
        mBaseCallback = new __WNInternal::__WNCallbackClasslessInternal0<Return>(_function);

        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }
    }

    template <typename Return>
    template <typename Class>
    WN_FORCE_INLINE WNCallback0<Return>::WNCallback0(Class* _this, Return (Class::*_function)()) {
        mBaseCallback = new __WNInternal::__WNCallbackInternal0<Class, Return>(_this, _function);

        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }
    }

    template <typename Return>
    WN_FORCE_INLINE WNCallback0<Return>::~WNCallback0() {
        if (mBaseCallback) {
            if (mBaseCallback->RemoveRef() == 0) {
                WN_DELETE(mBaseCallback);
            }
        }
    }

    template<typename Return>
    WN_FORCE_INLINE WNCallback0<Return>& WNCallback0<Return>::operator = (const WNCallback0<Return>& _callback) {
        if (mBaseCallback) {
            if (mBaseCallback->RemoveRef() == 0) {
                WN_DELETE(mBaseCallback);
            }
        }

        mBaseCallback = _callback.mBaseCallback;

        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }

        return(*this);
    }

    template<typename Return>
    WN_FORCE_INLINE Return WNCallback0<Return>::Execute() const {
        WN_DEBUG_ASSERT(mBaseCallback != WN_NULL);

        return(mBaseCallback->Execute());
    }
 
    template<typename Return>
    WN_FORCE_INLINE WN_BOOL WNCallback0<Return>::IsValid() {
        return(mBaseCallback != WN_NULL);
    }

    template <typename Return, typename Parameter>
    WN_FORCE_INLINE WNCallback1<Return, Parameter>::WNCallback1() :
        mBaseCallback(WN_NULL) {
    }

    template <typename Return, typename Parameter>
    WN_FORCE_INLINE WNCallback1<Return, Parameter>::WNCallback1(const WNCallback1<Return, Parameter>& _callback) :
        mBaseCallback(_callback.mBaseCallback) {
        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }
    }

    template <typename Return, typename Parameter>
    WN_FORCE_INLINE WNCallback1<Return, Parameter>::WNCallback1(Return (*_function)(Parameter)) {
        mBaseCallback = WN_NEW __WNInternal::__WNCallbackClasslessInternal1<Return, Parameter>(_function);

        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }
    }

    template <typename Return, typename Parameter>
    template <typename Class>
    WN_FORCE_INLINE WNCallback1<Return, Parameter>::WNCallback1(Class* _this, Return (Class::*_function)(Parameter)) {
        mBaseCallback = WN_NEW __WNInternal::__WNCallbackInternal1<Class, Return, Parameter>(_this, _function);

        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }
    }

    template <typename Return, typename Parameter>
    WN_FORCE_INLINE WNCallback1<Return, Parameter>::~WNCallback1() {
        if (mBaseCallback) {
            if (mBaseCallback->RemoveRef() == 0) {
                WN_DELETE(mBaseCallback);
            }
        }
    }

    template<typename Return, typename Parameter>
    WN_FORCE_INLINE WNCallback1<Return, Parameter>& WNCallback1<Return, Parameter>::operator = (const WNCallback1<Return, Parameter>& _callback) {
        if (mBaseCallback) {
            if (mBaseCallback->RemoveRef() == 0) {
                WN_DELETE(mBaseCallback);
            }
        }

        mBaseCallback = _callback.mBaseCallback;

        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }

        return(*this);
    }

    template<typename Return, typename Parameter>
    WN_FORCE_INLINE Return WNCallback1<Return, Parameter>::Execute(const Parameter& _parameter) const {
        WN_DEBUG_ASSERT(mBaseCallback != WN_NULL);

        return(mBaseCallback->Execute(_parameter));
    }
    
    template<typename Return, typename Parameter1>
    WN_FORCE_INLINE WN_BOOL WNCallback1<Return, Parameter1>::IsValid() {
        return(mBaseCallback != WN_NULL);
    }

    template<typename Return, typename Parameter1, typename Parameter2>
    WN_FORCE_INLINE WNCallback2<Return, Parameter1, Parameter2>::WNCallback2() :
        mBaseCallback(WN_NULL) {
    }

    template<typename Return, typename Parameter1, typename Parameter2>
    WN_FORCE_INLINE WNCallback2<Return, Parameter1, Parameter2>::WNCallback2(const WNCallback2<Return, Parameter1, Parameter2>& _callback) :
        mBaseCallback(_callback.mBaseCallback) {
        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }
    }

    template<typename Return, typename Parameter1, typename Parameter2>
    WN_FORCE_INLINE WNCallback2<Return, Parameter1, Parameter2>::WNCallback2(Return (*_function)(Parameter1, Parameter2)) {
        mBaseCallback = WN_NEW __WNInternal::__WNCallbackClasslessInternal2<Return, Parameter1, Parameter2>(_function);

        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }
    }

    template<typename Return, typename Parameter1, typename Parameter2>
    template<typename Class>
    WN_FORCE_INLINE WNCallback2<Return, Parameter1, Parameter2>::WNCallback2(Class* _this, Return (Class::*_function)(Parameter1, Parameter2)) {
        mBaseCallback =  WN_NEW __WNInternal::__WNCallbackInternal2<Class, Return, Parameter1, Parameter2>(_this, _function);

        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }
    }

    template<typename Return, typename Parameter1, typename Parameter2>
    WN_FORCE_INLINE WNCallback2<Return, Parameter1, Parameter2>::~WNCallback2() {
        if (mBaseCallback) {
            if (mBaseCallback->RemoveRef() == 0) {
                WN_DELETE(mBaseCallback);
            }
        }
    }

    template<typename Return, typename Parameter1, typename Parameter2>
    WN_FORCE_INLINE WNCallback2<Return, Parameter1, Parameter2>& WNCallback2<Return, Parameter1, Parameter2>::operator = (const WNCallback2<Return, Parameter1, Parameter2>& _callback) {
        if (mBaseCallback) {
            if (mBaseCallback->RemoveRef() == 0) {
                WN_DELETE(mBaseCallback);
            }
        }

        mBaseCallback = _callback.mBaseCallback;

        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }

        return(*this);
    }

    template<typename Return, typename Parameter1, typename Parameter2>
    WN_FORCE_INLINE Return WNCallback2<Return, Parameter1, Parameter2>::Execute(const Parameter1& _parameter1, const Parameter2& _parameter2) const {
        WN_DEBUG_ASSERT(mBaseCallback != WN_NULL);

        return(mBaseCallback->Execute(_parameter1, _parameter2));
    }

    template<typename Return, typename Parameter1, typename Parameter2>
    WN_FORCE_INLINE WN_BOOL WNCallback2<Return, Parameter1, Parameter2>::IsValid() {
        return(mBaseCallback != WN_NULL);
    }

    template<typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
    WN_FORCE_INLINE WNCallback3<Return, Parameter1, Parameter2, Parameter3>::WNCallback3() :
        mBaseCallback(WN_NULL) {
    }

    template<typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
    WN_FORCE_INLINE WNCallback3<Return, Parameter1, Parameter2, Parameter3>::WNCallback3(const WNCallback3<Return, Parameter1, Parameter2, Parameter3>& _callback) : 
        mBaseCallback(_callback.mBaseCallback) {
        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }
    }

    template<typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
    WN_FORCE_INLINE WNCallback3<Return, Parameter1, Parameter2, Parameter3>::WNCallback3(Return (*_function)(Parameter1, Parameter2, Parameter3)) {
        mBaseCallback = WN_NEW __WNInternal::__WNCallbackClasslessInternal3<Return, Parameter1, Parameter2, Parameter3>(_function);

        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }
    }

    template<typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
    template<typename Class>
    WN_FORCE_INLINE WNCallback3<Return, Parameter1, Parameter2, Parameter3>::WNCallback3(Class* _this, Return (Class::*_function)(Parameter1, Parameter2, Parameter3)) {
        mBaseCallback =  WN_NEW __WNInternal::__WNCallbackInternal3<Class, Return, Parameter1, Parameter2, Parameter3>(_this, _function);

        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }
    }

    template<typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
    WN_FORCE_INLINE WNCallback3<Return, Parameter1, Parameter2, Parameter3>::~WNCallback3() {
        if (mBaseCallback) {
            if (mBaseCallback->RemoveRef() == 0) {
                WN_DELETE(mBaseCallback);
            }
        }
    }

    template<typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
    WN_FORCE_INLINE WNCallback3<Return, Parameter1, Parameter2, Parameter3>& WNCallback3<Return, Parameter1, Parameter2, Parameter3>::operator = (const WNCallback3<Return, Parameter1, Parameter2, Parameter3>& _callback) {
        if (mBaseCallback) {
            if (mBaseCallback->RemoveRef() == 0) {
                WN_DELETE(mBaseCallback);
            }
        }

        mBaseCallback = _callback.mBaseCallback;

        if (mBaseCallback) {
            mBaseCallback->AddRef();
        }

        return(*this);
    }

    template<typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
    WN_FORCE_INLINE Return WNCallback3<Return, Parameter1, Parameter2, Parameter3>::Execute(const Parameter1& _parameter1, const Parameter2& _parameter2, const Parameter3& _parameter3) const {
        WN_DEBUG_ASSERT(mBaseCallback != WN_NULL);

        return(mBaseCallback->Execute(_parameter1, _parameter2, _parameter3));
    }

    template<typename Return, typename Parameter1, typename Parameter2, typename Parameter3>
    WN_FORCE_INLINE WN_BOOL WNCallback3<Return, Parameter1, Parameter2, Parameter3>::IsValid() {
        return(mBaseCallback != WN_NULL);
    }
}

#endif // __WN_CONTAINERS_CALLBACK_INL__
