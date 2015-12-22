set(INC_INTERNAL
  inc/Internal/WNConfig.h
)

source_group("inc\\Internal" FILES ${INC_INTERNAL})

add_wn_header_library(WNThreads
  inc/WNCallbackTask.h
  inc/WNConditionVariable.h
  inc/WNConfig.h
  inc/WNMutex.h
  inc/WNSemaphore.h
  inc/WNSharedMutex.h
  inc/WNSpinLock.h
  inc/WNThread.h
  inc/WNThreadPool.h
  inc/WNThreadTask.h
  ${INC_INTERNAL}
)

add_subdirectory(test)
