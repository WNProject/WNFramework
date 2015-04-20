// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_THREAD_H__
#define __WN_CONCURRENCY_THREAD_H__

#include "WNCore/inc/WNUtility.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNUtility.h"
#include "WNMemory/inc/WNIntrusivePtrBase.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

#if defined _WN_WINDOWS || defined _WN_ANDROID
    #include <mutex>
#endif
#if defined _WN_POSIX
    #include <unistd.h>
    #include <sys/syscall.h>
    #include <atomic>
#endif
#include <chrono>
#include <functional>

namespace wn {
    namespace internal {
        namespace concurrency {
            struct thread_data_base : memory::intrusive_ptr_base {
                WN_FORCE_INLINE thread_data_base() :
                    memory::intrusive_ptr_base(),
                    #ifdef _WN_WINDOWS
                        m_handle(NULL),
                    #endif
                    m_id(0) {
                #ifdef _WN_POSIX
                      m_joined.clear(std::memory_order_release);
                #endif
                }

                WN_FORCE_INLINE ~thread_data_base() {
                    #ifdef _WN_WINDOWS
                        if (m_handle != NULL) {
                            const BOOL close_result = ::CloseHandle(m_handle);

                            WN_DEBUG_ASSERT_DESC(close_result != FALSE, "Failed to destory thread object");

                            #ifndef _WN_DEBUG
                                WN_UNUSED_ARGUMENT(close_result);
                            #endif
                        }
                    #endif
                }

                #ifdef _WN_WINDOWS
                    HANDLE m_handle;
                    DWORD m_id;
                #elif defined _WN_POSIX
                    pthread_t m_pthread;
                    long m_id;
                    std::atomic_flag m_joined;
                #endif
            };

            template <typename _Type>
            struct thread_data final : thread_data_base {
                typedef _Type value_type;

                value_type m_result;
            };

            template <>
            struct thread_data<wn_void> : thread_data_base {};
        }
    }

    template <typename _Result>
    class thread final : public non_copyable {
    public:
        static_assert(!std::is_reference<_Result>::value, "thread return type must not be a reference");
        static_assert(!std::is_const<_Result>::value, "thread return type must not be const");

        typedef _Result result_type;

        WN_FORCE_INLINE thread() = default;

        WN_FORCE_INLINE thread(thread&& _thread) :
            m_data(std::move(_thread.m_data)) {
        }

        template <typename _Function, typename... _Arguments>
        WN_FORCE_INLINE explicit thread(_Function&& _function, _Arguments&&... _arguments) {
            static_assert(is_same<result_type, result_of_t<_Function(_Arguments...)>>::value,
                          "thread function return type does not match thread return type");

            std::function<result_type()> function(std::bind(decay_copy(std::forward<_Function>(_function)),
                                                            decay_copy(std::forward<_Arguments>(_arguments))...));

            execute(std::move(function));
        }

        WN_FORCE_INLINE thread& operator = (thread&& _thread) {
            m_data = std::move(_thread.m_data);

            return(*this);
        }

        WN_FORCE_INLINE wn_bool joinable() const {
            return(m_data != wn_nullptr);
        }

        WN_FORCE_INLINE wn_bool join() {
            if (joinable()) {
                #ifdef _WN_WINDOWS
                    if (::WaitForSingleObject(m_data->m_handle, INFINITE) == WAIT_OBJECT_0) {
                        return(wn_true);
                    }
                #elif defined _WN_POSIX
                    if (!m_data->m_joined.test_and_set(std::memory_order_acquire)) {
                      if (::pthread_join(m_data->m_pthread, NULL) == 0) {
                          return(wn_true);
                      }
                    } else {
                      return(wn_true);
                    }
                #endif
            }

            return(wn_false);
        }

        template <typename _Type = result_type,
                  typename = enable_if_t<(is_same<_Type, result_type>::value && !is_same<_Type, wn_void>::value)>>
        WN_FORCE_INLINE wn_bool join(_Type& _execution_result) {
            const wn_bool join_result = join();

            if (join_result) {
                _execution_result = m_data->m_result;
            }

            return(join_result);
        }

        WN_FORCE_INLINE wn_void detach() {
            m_data.reset();
        }

        WN_FORCE_INLINE wn_void swap(thread& _thread) {
            m_data.swap(_thread.m_data);
        }

    private:
        #ifdef _WN_WINDOWS
            typedef LPVOID thread_argument_type;
            typedef DWORD thread_result_type;
        #elif defined _WN_POSIX
            typedef void* thread_argument_type;
            typedef void* thread_result_type;
        #endif

        template <typename _Type>
        using thread_data = internal::concurrency::thread_data<_Type>;

        struct thread_execution_data final {
            std::function<result_type()> m_function;
            memory::intrusive_ptr<internal::concurrency::thread_data<result_type>> m_data;
        };

        static thread_result_type WN_OSCALL_BEGIN execution_wrapper(thread_argument_type _argument) WN_OSCALL_END {
            const thread_execution_data* execution_data = reinterpret_cast<thread_execution_data*>(_argument);

            #ifdef _WN_POSIX
                execution_data->m_data->m_id = syscall(SYS_gettid);
            #endif

            execute_helper(execution_data);

            memory::destroy(execution_data);

            #ifdef _WN_WINDOWS
                return(0);
            #elif defined _WN_POSIX
                return(wn_nullptr);
            #endif
        }

        static WN_FORCE_INLINE wn_void execute_helper(const thread_execution_data* _execution_data) {
            thread_data<result_type>* data = _execution_data->m_data.get();

            data->m_result = _execution_data->m_function();
        }

        WN_INLINE wn_void execute(std::function<result_type()>&& _function) {
            thread_execution_data* execution_data = memory::construct<thread_execution_data>();

            if (execution_data != wn_nullptr) {
                execution_data->m_function = std::move(_function);

                memory::intrusive_ptr<thread_data<result_type>> data = memory::make_intrusive<thread_data<result_type>>();

                if (data != wn_nullptr) {
                    execution_data->m_data = data;

                    #ifdef _WN_WINDOWS
                        const HANDLE handle = ::CreateThread(NULL, 0,
                                                             static_cast<LPTHREAD_START_ROUTINE>(&execution_wrapper),
                                                             static_cast<LPVOID>(execution_data),
                                                             0, &(data->m_id));

                        if (handle != NULL) {
                            data->m_handle = handle;
                            m_data = std::move(data);
                        } else {
                            memory::destroy(execution_data);

                            WN_RELEASE_ASSERT_DESC(handle != NULL, "Failed to create thread");
                        }
                    #elif defined _WN_POSIX
                        typedef void *(*start_routine_t)(void *);

                        pthread_t pthread;
                        const int result = ::pthread_create(&pthread, NULL,
                                                            static_cast<start_routine_t>(execution_wrapper),
                                                            static_cast<void*>(execution_data));

                        if (result == 0) {
                            data->m_pthread = pthread;
                            m_data = std::move(data);
                        } else {
                            memory::destroy(execution_data);

                            WN_RELEASE_ASSERT_DESC(result == 0, "Failed to create thread");
                        }
                    #endif
                } else {
                    memory::destroy(execution_data);

                    WN_RELEASE_ASSERT_DESC(m_data != wn_nullptr, "Failed to allocate needed data for thread");
                }
            } else {
                WN_RELEASE_ASSERT_DESC(execution_data != wn_nullptr, "Failed to allocate needed execution data for thread");
            }
        }

        memory::intrusive_ptr<thread_data<result_type>> m_data;
    };

    template <>
    WN_FORCE_INLINE wn_void thread<wn_void>::execute_helper(const thread_execution_data* _execution_data) {
        _execution_data->m_function();
    }

    namespace this_thread {
        WN_INLINE wn_void yield() {
            #if defined _WN_WINDOWS || defined _WN_ANDROID
                static std::once_flag once;
                static wn_bool multi_threaded = wn_false;

                static const auto multi_threaded_test = [](wn_bool& multi_threaded) {
                    #ifdef _WN_WINDOWS
                        SYSTEM_INFO sysInfo = {0};

                        ::GetSystemInfo(&sysInfo);

                        multi_threaded = (sysInfo.dwNumberOfProcessors > 1);
                    #elif defined _WN_ANDROID
                        multi_threaded = (::sysconf(_SC_NPROCESSORS_ONLN) > 1);
                    #else
                        WN_UNUSED_ARGUMENT(multi_threaded);
                    #endif
                };

                std::call_once(once, multi_threaded_test, std::ref(multi_threaded));

                if (multi_threaded) {
                    #ifdef _WN_WINDOWS
                        ::YieldProcessor();
                    #elif defined _WN_GCC || defined _WN_CLANG
                        #ifdef _WN_X86
                            __asm__ __volatile__ ("rep; nop" : : : "memory");
                        #else
                            ::sched_yield();
                        #endif
                    #endif
                } else {
                    #ifdef _WN_WINDOWS
                        ::SwitchToThread();
                    #elif defined _WN_ANDROID
                        ::sched_yield();
                    #endif
                }
            #elif defined _WN_POSIX
                const int result = ::pthread_yield();

                WN_RELEASE_ASSERT(result == 0);
            #endif
        }

        template <typename _Representation, typename _Period>
        WN_INLINE wn_void sleep_for(const std::chrono::duration<_Representation, _Period>& _duration) {
            #ifdef _WN_WINDOWS
                ::Sleep(static_cast<DWORD>(std::chrono::duration_cast<std::chrono::milliseconds>(_duration).count()));
            #elif defined _WN_POSIX
                const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(_duration);
                const std::chrono::nanoseconds nano_sec = std::chrono::duration_cast<std::chrono::nanoseconds>(_duration) -
                                                          std::chrono::duration_cast<std::chrono::nanoseconds>(sec);
                timespec timeRequested = {0};

                timeRequested.tv_sec = static_cast<int>(sec.count());
                timeRequested.tv_nsec = static_cast<int>(nano_sec.count());

                while (::nanosleep(&timeRequested, &timeRequested) == -1) {
                    continue;
                }
            #endif
        }
    }
}

#endif // __WN_CONCURRENCY_THREAD_H__
