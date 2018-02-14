enable_overlay_directory()

add_wn_library(
  WNMultiTasking
  SOURCES
    inc/internal/mutex_base.h
    inc/internal/spin_lock_base.h
    inc/call_once.h
    inc/callback_task.h
    inc/fiber.h
    src/fiber.cpp
    inc/job_pool.h
    src/job_pool.cpp
    inc/job_signal.h
    src/job_signal.cpp
    inc/lock_guard.h
    inc/mutex.h
    inc/semaphore.h
    inc/shared_mutex.h
    inc/spin_lock.h
    inc/synchronized.h
    inc/thread.h
    src/thread.cpp
    inc/thread_pool.h
    inc/thread_task.h
)

wn_add_subdirectory(test)

post_enable_overlay_directory()
