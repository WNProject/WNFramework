wn_create_application_tests_from_list(
  TEST_PREFIX WNRuntimeFileSystem
  SOURCE_DIR test
  COMMON_SOURCES
    ${WNFramework_SOURCE_DIR}/Libraries/WNFileSystem/test/src/TestFiles.h
    ${WNFramework_SOURCE_DIR}/Libraries/WNFileSystem/test/src/TestFiles.cpp
  SOURCES
    WNFactory
    WNMapping
  LIBS
    WNFileSystem
)

# TODO(devpow112) This needs to be improved in the future
propagate_to_parent(WN_ALL_STATIC_LIBS)
propagate_to_parent(WN_ALL_EXECUTABLES)
propagate_to_parent(WN_ALL_APPLICATIONS)
propagate_to_parent(WN_ALL_SHARED_LIBS)