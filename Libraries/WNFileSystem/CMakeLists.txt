include(CheckSymbolExists)
include(CheckIncludeFile)

set(WN_TARGET_NAME WNFileSystem)

set(WN_SRC_WINDOWS
  src/Windows/WNFileWindows.h
  src/Windows/WNFileWindows.cpp
  src/Windows/WNMappingWindows.h
  src/Windows/WNMappingWindows.cpp
  src/Windows/WNHandle.h
  src/Windows/WNSystemPaths.h
  src/Windows/WNSystemPaths.cpp
)

set(WN_SRC_POSIX
  src/Posix/WNFileDescriptor.h
  src/Posix/WNFilePosix.h
  src/Posix/WNFilePosix.cpp
  src/Posix/WNMappingPosix.h
  src/Posix/WNMappingPosix.cpp
)

set(WN_SRC_POSIX_SYSTEM_PATHS
  src/Posix/WNSystemPaths.h
  src/Posix/WNSystemPaths.cpp
)

set(WN_SRC_ANDROID_SYSTEM_PATHS
  src/Android/WNSystemPaths.h
  src/Android/WNSystemPaths.cpp
)

set(WN_SRC
  src/WNFactory.cpp
  src/WNMapping.cpp
  src/WNMemoryBackedMapping.cpp
  src/WNMemoryBackedMapping.h
  src/WNUtilities.h
)

set(WN_FILE_SYSTEM_FILES ${WN_SRC})

if (MSVC OR WN_IS_TEGRA)
  list(APPEND WN_FILE_SYSTEM_FILES
    ${WN_SRC_WINDOWS}
    ${WN_SRC_POSIX}
    ${WN_SRC_POSIX_SYSTEM_PATHS}
    ${WN_SRC_ANDROID_SYSTEM_PATHS}
  )

  source_group("src\\Windows" FILES ${WN_SRC_WINDOWS})
  source_group("src\\Posix" FILES ${WN_SRC_POSIX})
  source_group("src\\Posix" FILES ${WN_SRC_POSIX_SYSTEM_PATHS})
  source_group("src\\Android" FILES ${WN_SRC_ANDROID_SYSTEM_PATHS})
  source_group("src" FILES ${WN_SRC})

  if (WN_PLATFORM STREQUAL "Windows")
    set_source_files_properties(
      ${WN_SRC_POSIX}
      PROPERTIES HEADER_FILE_ONLY true
    )

    set_source_files_properties(
      ${WN_SRC_POSIX_SYSTEM_PATHS}
      PROPERTIES HEADER_FILE_ONLY true
    )

    set_source_files_properties(
      ${WN_SRC_ANDROID_SYSTEM_PATHS}
      PROPERTIES HEADER_FILE_ONLY true
    )
  else()
    set_source_files_properties(
      ${WN_SRC_WINDOWS}
      PROPERTIES HEADER_FILE_ONLY true
    )

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
      set_source_files_properties(
        ${WN_SRC_POSIX_SYSTEM_PATHS}
        PROPERTIES HEADER_FILE_ONLY true
      )
    elseif (WN_POSIX)
      set_source_files_properties(
        ${WN_SRC_ANDROID_SYSTEM_PATHS}
        PROPERTIES HEADER_FILE_ONLY true
      )
    endif()
  endif()
endif()

if (WN_POSIX)
  if (NOT WN_IS_TEGRA)
    list(APPEND WN_FILE_SYSTEM_FILES ${WN_SRC_POSIX})

    if (ANDROID)
      list(APPEND WN_FILE_SYSTEM_FILES ${WN_SRC_ANDROID_SYSTEM_PATHS})
    else()
      list(APPEND WN_FILE_SYSTEM_FILES ${WN_SRC_POSIX_SYSTEM_PATHS})
    endif()
  endif()

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

    add_compile_options(-D_WN_HAS_MREMAP)
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

    add_compile_options(-D_WN_HAS_FALLOCATE)
  else()
    check_symbol_exists(posix_fallocate "fcntl.h" WN_HAS_POSIX_FALLOCATE_SYMBOL)

    if (WN_HAS_POSIX_FALLOCATE_SYMBOL)
      set(WN_HAS_FTRUNCATE_ALTERNATIVE true)

      add_compile_options(-D_WN_HAS_POSIX_FALLOCATE)
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

  set(CMAKE_REQUIRED_DEFINITIONS -D_XOPEN_SOURCE=500)

  check_symbol_exists(nftw "ftw.h" WN_HAS_NFTW_SYMBOL)

  set(CMAKE_REQUIRED_DEFINITIONS)

  if (WN_HAS_NFTW_SYMBOL)
    set(WN_HAS_READDIR_ALTERNATIVE true)

    add_compile_options(-D_XOPEN_SOURCE=500)
    add_compile_options(-D_WN_HAS_NFTW)
  endif()

  if (NOT WN_HAS_READDIR_ALTERNATIVE)
    check_symbol_exists(readdir "dirent.h" WN_HAS_READDIR_SYMBOL)

    if (WN_HAS_READDIR_SYMBOL)
      add_compile_options(-D_WN_HAS_READDIR)
    else()
      message(FATAL_ERROR "unable to locate required symbol readdir")
    endif()
  endif()

  if (WN_INCLUDE_GNU_SOURCE_DEFINE)
    add_compile_options(-D_GNU_SOURCE)
  endif()
endif()

add_wn_library(${WN_TARGET_NAME} SOURCES
  inc/WNFactory.h
  inc/WNFile.h
  inc/WNMapping.h
  inc/WNResult.h
  ${WN_FILE_SYSTEM_FILES}
)

wn_target_link_libraries(${WN_TARGET_NAME} WNUtils)

if (WN_SYSTEM STREQUAL "Windows")
  wn_target_link_libraries(${WN_TARGET_NAME} Rpcrt4.lib)
endif()

target_include_directories(${WN_TARGET_NAME} PUBLIC
  ${CMAKE_SOURCE_DIR}/Libraries
)

if (WN_SYSTEM STREQUAL "Android")
  set_property(TARGET ${WN_TARGET_NAME} PROPERTY WN_ANDROID_PERMISSIONS
    WRITE_EXTERNAL_STORAGE
    READ_EXTERNAL_STORAGE
  )
endif()

add_subdirectory(test)