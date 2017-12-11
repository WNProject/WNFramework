enable_overlay_directory()

add_wn_library(WNMultiTasking
  SOURCES
    inc/internal/mutex_base.h
    inc/internal/spin_lock_base.h
    inc/mutex.h
    inc/WNCallbackTask.h
    inc/WNCallOnce.h
    inc/WNFiber.h
    src/WNFiber.cpp
    inc/WNJobPool.h
    src/WNJobPool.cpp
    inc/WNJobSignal.h
    src/WNJobSignal.cpp
    inc/WNLockGuard.h
    inc/WNSemaphore.h
    inc/WNSharedMutex.h
    inc/WNSpinLock.h
    inc/WNSynchronized.h
    inc/WNThread.h
    src/WNThread.cpp
    inc/WNThreadPool.h
    inc/WNThreadTask.h
)

wn_add_subdirectory(test)

post_enable_overlay_directory()
