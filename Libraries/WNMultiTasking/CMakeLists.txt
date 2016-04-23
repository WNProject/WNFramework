enable_overlay_directory()

add_wn_library(WNMultiTasking
  SOURCES
    inc/WNCallbackTask.h
    inc/WNCallOnce.h
    inc/WNConditionVariable.h
    inc/WNFiber.h
    src/WNFiber.cpp
    inc/WNLockGuard.h
    inc/WNMutex.h
    inc/WNSemaphore.h
    inc/WNSharedMutex.h
    inc/WNSpinLock.h
    inc/WNThread.h
    src/WNThread.cpp
    inc/WNThreadPool.h
    inc/WNThreadTask.h
)

add_subdirectory(test)
