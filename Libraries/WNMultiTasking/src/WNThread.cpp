#include "WNMultiTasking/inc/WNThread.h"

namespace wn {
namespace multi_tasking {

void thread::create(
    memory::allocator* _allocator, functional::function<void()>&& _f) {
  memory::intrusive_ptr<private_data> data(
      memory::make_intrusive<private_data>(_allocator, _allocator));

  if (data) {
    private_execution_data* execution_data =
        _allocator->construct<private_execution_data>(
            _allocator, core::move(_f), data);

    if (execution_data) {
#ifdef _WN_WINDOWS
      DWORD id;
      const HANDLE handle =
          ::CreateThread(NULL, 0, static_cast<LPTHREAD_START_ROUTINE>(&wrapper),
              static_cast<LPVOID>(execution_data), 0, &id);
      const bool creation_success = handle != NULL;
#elif defined _WN_POSIX
      typedef void* (*start_routine_t)(void*);

      pthread_t pthread;
      const bool creation_success =
          ::pthread_create(&pthread, NULL,
              static_cast<start_routine_t>(&wrapper),
              static_cast<void*>(execution_data)) == 0;
#endif

      if (creation_success) {
#ifdef _WN_WINDOWS
        data->m_id = id;
        data->m_handle = handle;
#elif defined _WN_POSIX
        data->m_start_lock.wait();

        data->m_pthread = pthread;
#endif

        m_data = core::move(data);
      } else {
        _allocator->destroy(execution_data);

        WN_RELEASE_ASSERT_DESC(creation_success, "failed to create thread");
      }
    } else {
      WN_RELEASE_ASSERT_DESC(execution_data,
          "failed to allocate needed execution data for thread");
    }
  } else {
    WN_RELEASE_ASSERT_DESC(data, "failed to allocate needed data for thread");
  }
}

thread::result_type WN_OSCALL_BEGIN thread::wrapper(
    argument_type _arg) WN_OSCALL_END {
  private_execution_data* execution_data =
      static_cast<private_execution_data*>(_arg);

  WN_RELEASE_ASSERT_DESC(execution_data, "invalid thread execution data");

#ifdef _WN_POSIX
  private_data* data = execution_data->m_data.get();

  WN_RELEASE_ASSERT_DESC(data, "invalid thread data");

#ifdef _WN_LINUX
  data->m_id = ::syscall(SYS_gettid);
#elif defined _WN_ANDROID
  data->m_id = ::gettid();
#endif

  data->m_start_lock.notify();
#endif

  execution_data->m_function();

  memory::allocator* allocator = execution_data->m_allocator;

  allocator->destroy(execution_data);

#ifdef _WN_WINDOWS
  return 0;
#elif defined _WN_POSIX
  return NULL;
#endif
}

}  // namespace multi_tasking
}  // namespace wn