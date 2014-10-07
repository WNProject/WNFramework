////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_THREAD_POOL_INL__
#define __WN_CONCURRENCY_THREAD_POOL_INL__

#ifndef __WN_CONCURRENCY_THREAD_POOL_H__
    #error "WNThreadPool.inl should never be included directly. Please include WNThreadPool.h instead"
#endif

#ifdef _WN_WINDOWS
    #include <windows.h>
#endif

#include <algorithm>

namespace WNConcurrency {
    WN_FORCE_INLINE WNThreadPool::WNThreadPool() :
            mIOCPHandle(INVALID_HANDLE_VALUE),
        mNumThreads(0),
        mShutdown(WN_TRUE) {
    }

    WN_FORCE_INLINE WNThreadPool::~WNThreadPool() {
        Cleanup();
    }

    WN_INLINE WN_VOID WNThreadPool::JobRunnerThread(const WN_UINT32 _threadNum) {
        mThreadStartMutex.Post();

            LPOVERLAPPED overlapped = NULL;
            DWORD bytesTransferred = 0;
            ULONG_PTR completionKey = 0;

        for (;;) {
            const BOOL ret = GetQueuedCompletionStatus(mIOCPHandle, &bytesTransferred, &completionKey, &overlapped, INFINITE);

            WN_RELEASE_ASSERT_DESC(ret, "Failed to get completion status from the queue");

            if (0 == completionKey) {
                // Message from the pool, not an actual job
                switch (bytesTransferred) {
                    case 0:
                        // Cleanup Message (We should be responsible for freeing memory)
                        mThreadMutex.Lock();

                        WN_DELETE(mThreads[_threadNum]);

                        mThreadFreelist.push_back(_threadNum);
                        mThreadMutex.Unlock();

                        return;
                    case 1:
                        // Cleanup Message (Pool will worry about freeing memory)
                        return;
                    default:
                        WN_RELEASE_ASSERT_DESC(WN_FALSE, "This isn't right, we got a message from the queue we don't understand");

                        break;
                }
            } else {
                WNResourcePointer<WNThreadJob>* job = reinterpret_cast<WNResourcePointer<WNThreadJob>* >(completionKey);

                (*job)->Execute();
                (*job)->mComplete = WN_TRUE;

                WN_DELETE(job);
            }
       }
    }

    WN_INLINE WN_VOID WNThreadPool::Cleanup() {
        mShutdown = WN_TRUE;

        mThreadMutex.Lock();

        for (WN_SIZE_T i = 0; i < mThreads.size(); ++i) {
            if (std::find(mThreadFreelist.begin(), mThreadFreelist.end(), i) == mThreadFreelist.end()) {
                PostQueuedCompletionStatus(mIOCPHandle, 1, NULL, 0);
            }
        }

        for (WN_SIZE_T i = 0; i < mThreads.size(); ++i) {
            if (std::find(mThreadFreelist.begin(), mThreadFreelist.end(), i) == mThreadFreelist.end()) {
                mThreads[i]->WaitForCompletion();

                WN_DELETE(mThreads[i]);
            }
        }

        mThreads.clear();
        mThreadFreelist.clear();
        mThreadMutex.Unlock();

        CloseHandle(mIOCPHandle);

        mIOCPHandle = INVALID_HANDLE_VALUE;
   }

    WN_INLINE WNThreadPoolReturnCode::Type WNThreadPool::Initialize(const WN_UINT32 _numWorkers) {
        if (mShutdown == WN_FALSE) {
            return(WNThreadPoolReturnCode::eWNReInitialized);
        }

        mIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

        if (INVALID_HANDLE_VALUE == mIOCPHandle) {
            return(WNThreadPoolReturnCode::eWNError);
        }

        for (WN_UINT32 i = 0; i < _numWorkers; ++i) {
            WNThread<WN_VOID>* thread = WN_NEW WNThread<WN_VOID>();

            if (thread->Initialize(this, &WNThreadPool::JobRunnerThread, i) != WNThreadReturnCode::eWNOK) {
                WN_DELETE(thread);

                Cleanup();

                return(WNThreadPoolReturnCode::eWNInvalidParameters);
            }

            mThreads.push_back(thread);
            mThreadStartMutex.Wait();
        }

        mNumThreads = _numWorkers;
        mShutdown = WN_FALSE;

        return(WNThreadPoolReturnCode::eWNOK);
    }

    WN_INLINE WNThreadPoolReturnCode::Type WNThreadPool::AddJob(const WNResourcePointer<WNThreadJob>& _job) {
        //The job is responsible for deleting this data
        //At some point when WNMemory is more completed, should probably switch this to a bucketed allocator (reduce fragmentation, and allocation/free time)
        WNResourcePointer<WNThreadJob>* mJob = WN_NEW WNResourcePointer<WNThreadJob>(_job);

        if (!PostQueuedCompletionStatus(mIOCPHandle, 0, reinterpret_cast<ULONG_PTR>(mJob), 0)) {
            WN_DELETE(mJob);

            return(WNThreadPoolReturnCode::eWNUninitialized);
        }
        return(WNThreadPoolReturnCode::eWNOK);
    }
}
#endif // __WN_CONCURRENCY_THREAD_POOL_INL__
