enable_overlay_directory()

add_wn_library(WNMultiTasking
  SOURCES
    inc/internal/mutex_base.h
    inc/internal/spin_lock_base.h
    inc/call_once.h
    inc/fiber.h
    src/fiber.cpp
    inc/lock_guard.h
    inc/mutex.h
    inc/semaphore.h
    inc/shared_mutex.h
    inc/spin_lock.h
    inc/thread.h
    src/thread.cpp
    inc/WNCallbackTask.h
    inc/WNJobPool.h
    src/WNJobPool.cpp
    inc/WNJobSignal.h
    src/WNJobSignal.cpp
    inc/WNSynchronized.h
    inc/WNThreadPool.h
    inc/WNThreadTask.h
)

wn_add_subdirectory(test)

post_enable_overlay_directory()
