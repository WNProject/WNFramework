////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_LOCK_GUARD_H__
#define __WN_CONCURRENCY_LOCK_GUARD_H__

#include "WNCore/inc/WNTypes.h"

namespace WNConcurrency {
    template <typename Lock>
    class WNLockGuard {
    public:
        explicit WNLockGuard(Lock& _lock, const WN_BOOL _adoptLock = WN_FALSE);
        ~WNLockGuard();

    private:
        WNLockGuard(const WNLockGuard&);
        WNLockGuard& operator = (const WNLockGuard&);

    private:
        Lock& mLock;
    };
}

#include "WNConcurrency/inc/Internal/WNLockGuard.inl"

#endif // __WN_CONCURRENCY_LOCK_GUARD_H__