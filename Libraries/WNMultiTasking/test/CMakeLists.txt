wn_create_executable_tests_from_list(
  TEST_PREFIX multi_tasking
  SOURCE_DIR src
  SOURCES
    call_once
    fiber
    job_pool
    mutex
    semaphore
    shared_mutex
    spin_lock
    synchronized
    thread_pool
    thread
  LIBS
    WNMultiTasking
)
