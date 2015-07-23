wn_create_tests_from_list(TEST_PREFIX WNMemory
  COMMON_SOURCES src/Main.cpp
  SOURCE_DIR src/Tests
  SOURCES
    WNAllocator
    WNBasic
    WNIntrusivePtr
    WNStringUtility
    WNUniquePtr
)
