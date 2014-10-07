////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_THREAD_POOL_LINUX_INL__
#define __WN_CONCURRENCY_THREAD_POOL_LINUX_INL__
#ifndef __WN_CONCURRENCY_THREAD_POOL_H__
    #error "WNThreadPoolLinux.inl should never be included directly. Please include WNThreadPool.h instead"
#endif

#include <algorithm>

namespace WNConcurrency {
    WN_FORCE_INLINE WNThreadPool::WNThreadPool(): mShutdown(WN_TRUE) {
        
    }

    WN_FORCE_INLINE WNThreadPool::~WNThreadPool() {
        Cleanup();
    }

    WN_INLINE WN_VOID WNThreadPool::JobRunnerThread(const WN_UINT32 _threadNum)  {
        mThreadStartMutex.Post();
        for(;;) {
            mJobsAvailable.Wait();
            mJobMutex.Lock();
            WNResourcePointer<WNThreadJob> job = mThreadJobs.front();
            mThreadJobs.pop_front();
            mJobMutex.Unlock();
            if(!job) { //we were told to return
                return;
            }
            job->Execute();
        };
    }

    WN_INLINE WN_VOID WNThreadPool::Cleanup()  {
        mShutdown = WN_TRUE;
        for(WN_SIZE_T i = 0; i < mThreads.size(); ++i ){
            mThreadJobs.push_back(WNResourcePointer<WNThreadJob>());
            mJobsAvailable.Post();
        }
        for(WN_SIZE_T i = 0; i < mThreads.size(); ++i) {
           mThreads[i]->WaitForCompletion();
           WN_DELETE(mThreads[i]);
        }
        mThreads.clear();
        mThreadJobs.clear();
    }

    WN_INLINE WNThreadPoolReturnCode::Type WNThreadPool::Initialize(const WN_UINT32 _numWorkers) {
        if(mShutdown == WN_FALSE) {
            return(WNThreadPoolReturnCode::eWNReInitialized);
        }
        for(WN_UINT32 i = 0; i < _numWorkers; ++i) {
            WNThread<WN_VOID>* thread = WN_NEW WNThread<WN_VOID>();
            if(WNThreadReturnCode::eWNOK != thread->Initialize(this, &WNThreadPool::JobRunnerThread, i)) {
                WN_DELETE(thread);
                Cleanup();
                return(WNThreadPoolReturnCode::eWNInvalidParameters);
            }
            mThreads.push_back(thread);
            mThreadStartMutex.Wait();
        }
        mShutdown = WN_FALSE;
        return(WNThreadPoolReturnCode::eWNOK);
    }

    WN_INLINE WNThreadPoolReturnCode::Type WNThreadPool::AddJob(const WNResourcePointer<WNThreadJob>& _job) {
        if(!_job || mShutdown) {
            return(WNThreadPoolReturnCode::eWNInvalidParameters);
        }
        mJobMutex.Lock();
        mThreadJobs.push_back(_job);
        mJobMutex.Unlock();
        mJobsAvailable.Post();
        return(WNThreadPoolReturnCode::eWNOK);
    }
}

#endif // __WN_CONCURRENCY_THREAD_POOL_INL__
