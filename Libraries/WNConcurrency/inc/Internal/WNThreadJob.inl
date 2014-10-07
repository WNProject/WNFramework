////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_THREAD_JOB_INL__
#define __WN_CONCURRENCY_THREAD_JOB_INL__

#ifndef __WN_CONCURRENCY_THREAD_JOB_H__
    #error "WNThreadJob.inl should never be included directly. Please include WNThreadJob.h instead"
#endif

#include "WNConcurrency/inc/WNThread.h"

namespace WNConcurrency {
    WN_FORCE_INLINE WNThreadJob::WNThreadJob() :
        WNConcurrency::WNResourceBase(),
        mComplete(WN_FALSE) {
    }

    WN_FORCE_INLINE WN_BOOL WNThreadJob::IsComplete() const {
        return(mComplete);
    }

    WN_FORCE_INLINE WN_VOID WNThreadJob::WaitForCompletion() {
        mCompletedSemaphore.Wait();
        mCompletedSemaphore.Post(); //In case anyone else is waiting for us to be completed
    }

    WN_FORCE_INLINE WN_VOID WNThreadJob::MarkCompleted() {
       mCompletedSemaphore.Post();
       mComplete = true;
    }
}

#endif // __WN_CONCURRENCY_THREAD_JOB_INL__
