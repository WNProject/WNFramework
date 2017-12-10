set(ENABLED OFF)

wn_check_symbol_exists(pthread_spin_init "pthread.h" WN_HAS_PTHREAD_SPIN_LOCK)

if (WN_HAS_PTHREAD_SPIN_LOCK)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} SpinLock)
