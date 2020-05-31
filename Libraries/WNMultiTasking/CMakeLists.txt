enable_overlay_directory()

add_wn_library(
  WNMultiTasking
  SOURCES
    inc/internal/mutex_base.h
    inc/internal/spin_lock_base.h
    inc/internal/thread_base_common.h
    inc/call_once.h
    inc/callback_task.h
    inc/condition_variable.h
    inc/fiber.h
    inc/job_pool.h
    inc/job_signal.h
    inc/lock_guard.h
    inc/mutex.h
    inc/semaphore.h
    inc/shared_mutex.h
    inc/spin_lock.h
    inc/synchronized.h
    inc/thread.h
    inc/thread_pool.h
    inc/thread_task.h
    inc/utilities.h
    inc/work_queue.h
    src/fiber.cpp
    src/job_pool.cpp
    src/job_signal.cpp
    src/thread.cpp
)

wn_add_subdirectory(test)

post_enable_overlay_directory()
