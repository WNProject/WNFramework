enable_overlay_directory()

add_wn_library(
  WNMultiTasking
  INCLUDES
    ${WNFramework_SOURCE_DIR}/externals/marl/src/
  SOURCES
    inc/internal/mutex_base.h
    inc/internal/spin_lock_base.h
    inc/internal/thread_base_common.h
    inc/call_once.h
    inc/callback_task.h
    inc/condition_variable.h
    inc/job_pool.h
    inc/lock_guard.h
    inc/mutex.h
    inc/semaphore.h
    inc/shared_mutex.h
    inc/spin_lock.h
    inc/thread.h
    inc/thread_pool.h
    inc/thread_task.h
    inc/utilities.h
    inc/work_queue.h
    src/job_pool.cpp
    src/job_pool_thread_locals.cpp
    src/thread.cpp
  LIBS
    marl
)

wn_add_subdirectory(test)

post_enable_overlay_directory()
