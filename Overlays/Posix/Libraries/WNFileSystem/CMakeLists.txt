include(CheckSymbolExists)
include(CheckIncludeFile)

set(compile_opts)

check_include_file("sys/types.h" WN_HAS_SYS_TYPES_H)

if (NOT WN_HAS_SYS_TYPES_H)
  message(FATAL_ERROR "unable to locate required include sys/types.h")
endif()

check_symbol_exists(fstat "sys/stat.h" WN_HAS_FSTAT_SYMBOL)

if (NOT WN_HAS_FSTAT_SYMBOL)
  message(FATAL_ERROR "unable to locate required symbol fstat")
endif()

set(WN_INCLUDE_GNU_SOURCE_DEFINE false)

check_symbol_exists(mmap "sys/mman.h" WN_HAS_MMAP_SYMBOL)

if (NOT WN_HAS_MMAP_SYMBOL)
  message(FATAL_ERROR "unable to locate required symbol mmap")
endif()

check_symbol_exists(munmap "sys/mman.h" WN_HAS_MUNMAP_SYMBOL)

if (NOT WN_HAS_MUNMAP_SYMBOL)
  message(FATAL_ERROR "unable to locate required symbol munmap")
endif()

set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)

check_symbol_exists(mremap "sys/mman.h" WN_HAS_MREMAP_SYMBOL)

set(CMAKE_REQUIRED_DEFINITIONS)

if (WN_HAS_MREMAP_SYMBOL)
  set(WN_INCLUDE_GNU_SOURCE_DEFINE true)

  list(APPEND compile_opts -D_WN_HAS_MREMAP)
endif()

check_include_file("fcntl.h" WN_HAS_FCNTL_H)

set(WN_HAS_FTRUNCATE_ALTERNATIVE false)

check_symbol_exists(open "fcntl.h" WN_HAS_OPEN_SYMBOL)

if (NOT WN_HAS_OPEN_SYMBOL)
  message(FATAL_ERROR "unable to locate required symbol open")
endif()

set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)

check_symbol_exists(fallocate "fcntl.h" WN_HAS_FALLOCATE_SYMBOL)

set(CMAKE_REQUIRED_DEFINITIONS)

if (WN_HAS_FALLOCATE_SYMBOL)
  set(WN_INCLUDE_GNU_SOURCE_DEFINE true)
  set(WN_HAS_FTRUNCATE_ALTERNATIVE true)

  list(APPEND compile_opts -D_WN_HAS_FALLOCATE)
else()
  check_symbol_exists(posix_fallocate "fcntl.h" WN_HAS_POSIX_FALLOCATE_SYMBOL)

  if (WN_HAS_POSIX_FALLOCATE_SYMBOL)
    set(WN_HAS_FTRUNCATE_ALTERNATIVE true)

    list(APPEND compile_opts -D_WN_HAS_POSIX_FALLOCATE)
  endif()
endif()

check_symbol_exists(close "unistd.h" WN_HAS_CLOSE_SYMBOL)

if (NOT WN_HAS_CLOSE_SYMBOL)
  message(FATAL_ERROR "unable to locate required symbol close")
endif()

if (NOT WN_HAS_FTRUNCATE_ALTERNATIVE)
  check_symbol_exists(ftruncate "unistd.h" WN_HAS_FTRUNCATE_SYMBOL)

  if (NOT WN_HAS_FTRUNCATE_SYMBOL)
    message(FATAL_ERROR "unable to locate required symbol ftruncate")
  endif()
endif()

set(WN_HAS_FTS_ALTERNATIVE false)

set(CMAKE_REQUIRED_DEFINITIONS -D_XOPEN_SOURCE=600)

check_symbol_exists(nftw "ftw.h" WN_HAS_NFTW_SYMBOL)

set(CMAKE_REQUIRED_DEFINITIONS)

if (WN_HAS_NFTW_SYMBOL)
  set(WN_HAS_READDIR_ALTERNATIVE true)

  list(APPEND compile_opts -D_WN_HAS_NFTW)
endif()

if (NOT WN_HAS_READDIR_ALTERNATIVE)
  check_symbol_exists(readdir "dirent.h" WN_HAS_READDIR_SYMBOL)

  if (WN_HAS_READDIR_SYMBOL)
    list(APPEND compile_opts -D_WN_HAS_READDIR)
  else()
    message(FATAL_ERROR "unable to locate required symbol readdir")
  endif()
endif()

if (WN_INCLUDE_GNU_SOURCE_DEFINE)
  list(APPEND compile_opts -D_GNU_SOURCE)
endif()

overlay_library_sources(WNFileSystem
  SOURCES
    src/WNFileDescriptor.h
    src/WNFilePosix.cpp
    src/WNFilePosix.h
    src/WNMappingPosix.cpp
    src/WNMappingPosix.h
  COMPILE_OPTIONS
    ${compile_opts}
)
