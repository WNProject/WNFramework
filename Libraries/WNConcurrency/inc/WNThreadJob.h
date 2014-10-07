////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_THREAD_JOB_H__
#define __WN_CONCURRENCY_THREAD_JOB_H__

#include "WNCore/inc/WNTypes.h"
#include "WNConcurrency/inc/WNResourceBase.h"
#include "WNConcurrency/inc/WNSemaphore.h"

namespace WNConcurrency {
    class WNThreadPool;

    class WNThreadJob : public WNConcurrency::WNResourceBase {
    public:
        WNThreadJob();
        virtual WN_FORCE_INLINE ~WNThreadJob() {}

        virtual WN_VOID Execute() = 0;

        WN_BOOL IsComplete() const;

        WN_VOID WaitForCompletion();

    private:
        friend class WNThreadPool;
    protected:
        WN_VOID MarkCompleted();
    private:
        WN_BOOL mComplete;
        WNSemaphore mCompletedSemaphore;
    };
}

#include "WNConcurrency/inc/Internal/WNThreadJob.inl"

#endif // __WN_CONCURRENCY_THREAD_JOB_H__
