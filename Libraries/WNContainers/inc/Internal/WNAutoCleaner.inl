////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONTAINERS_INTERNAL_AUTO_CLEANER_INL__
#define __WN_CONTAINERS_INTERNAL_AUTO_CLEANER_INL__

namespace WNContainers {
    template<typename T, WN_VOID (*F)(T&)>
    WN_FORCE_INLINE WNAutoCleaner<T, F>::WNAutoCleaner(T& _object) :
        mObject(_object),
        mAttached(WN_TRUE) {
    }

    template<typename T, WN_VOID (*F)(T&)>
    WN_FORCE_INLINE WNAutoCleaner<T, F>::~WNAutoCleaner() {
        if (mAttached) {
            F(mObject);
        }
    }

    template<typename T, WN_VOID (*F)(T&)>
    WN_FORCE_INLINE WN_VOID WNAutoCleaner<T, F>::Detach() {
        mAttached = WN_FALSE;
    }

    template<typename T, WN_VOID (*F)(T&)>
    WN_FORCE_INLINE WNAutoCleaner<T, F>::operator T& () const {
        return(mObject);
    }

    template<typename T, WN_VOID (*F)(T&)>
    WN_FORCE_INLINE WN_VOID WNAutoCleaner<T, F>::operator = (const T& _other) {
        mObject = _other;
    }

    template<typename T, WN_VOID (*F)(T&)>
    WN_FORCE_INLINE WN_BOOL WNAutoCleaner<T, F>::operator == (const T& _other) const {
        return(mObject == _other);
    }
}

#endif // __WN_CONTAINERS_INTERNAL_AUTO_CLEANER_INL__
