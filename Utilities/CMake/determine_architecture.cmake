include(CheckTypeSize)
include(CheckSymbolExists)
include(TestBigEndian)

# check system architecture using cmake system variables
if (DEFINED CMAKE_SYSTEM_PROCESSOR AND NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "")
  if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR
      ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "amd64")
    set(WN_ARCHITECTURE "x86-64")
  elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
    # CMake reports AMD64 on Windows always need to check for bit width
    if (DEFINED CMAKE_CL_64 AND CMAKE_CL_64)
      set(WN_ARCHITECTURE "x86-64")
    else()
      set(WN_ARCHITECTURE "x86")
    endif()
  elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86" OR
          ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386" OR
          ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
    set(WN_ARCHITECTURE "x86")
  elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm" OR
          ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "armv5te" OR
          ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "armv6" OR
          ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "armv7-a")
    set(WN_ARCHITECTURE "ARM")
  elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    set(WN_ARCHITECTURE "ARM64")
  endif()
endif()

function(wn_check_any_symbol_exists VARIABLE)
  foreach(SYMBOL ${ARGN})
    set(CHECK_VARIABLE_NAME HAS_${SYMBOL})
    string(TOUPPER ${CHECK_VARIABLE_NAME} CHECK_VARIABLE_NAME)

    check_symbol_exists(${SYMBOL} "" ${CHECK_VARIABLE_NAME})

    if (${CHECK_VARIABLE_NAME})
      set(${VARIABLE} true PARENT_SCOPE)
      break()
    else()
      set(${VARIABLE} false PARENT_SCOPE)
    endif()
  endforeach()
endfunction()

# check for x86-64 using processor preprocessor defines if not already found
if (NOT DEFINED WN_ARCHITECTURE)
  set(X86_64_SYMBOLS
    __amd64__
    __amd64
    __x86_64__
    __x86_64
    _M_X64
    _M_AMD64
  )

  wn_check_any_symbol_exists(IS_X86_64 ${X86_64_SYMBOLS})

  if (IS_X86_64)
    set(WN_ARCHITECTURE "x86-64")
  endif()
endif()

# check for x86 using processor preprocessor defines if not already found
if (NOT DEFINED WN_ARCHITECTURE)
  set(X86_SYMBOLS
    i386
    __i386
    __i386__
    __IA32__
    _M_I86
    _M_IX86
    __X86__
    _X86_
    __THW_INTEL__
    __I86__
    __INTEL__
    __386
  )

  wn_check_any_symbol_exists(IS_X86 ${X86_SYMBOLS})

  if (IS_X86)
    set(WN_ARCHITECTURE "x86")
  endif()
endif()

# check for ARM using processor preprocessor defines if not already found
if (NOT DEFINED WN_ARCHITECTURE)
  set(ARM_SYMBOLS
    __arm__
    __arm
    __thumb__
    __TARGET_ARCH_ARM
    __TARGET_ARCH_THUMB
    _ARM
    _M_ARM
    _M_ARMT
  )

  wn_check_any_symbol_exists(IS_ARM ${ARM_SYMBOLS})

  if (IS_ARM)
    set(WN_ARCHITECTURE "ARM")
  endif()
endif()

# check for ARM64 using processor preprocessor defines if not already found
if (NOT DEFINED WN_ARCHITECTURE)
  set(ARM64_SYMBOLS __aarch64__)

  wn_check_any_symbol_exists(IS_ARM64 ${ARM64_SYMBOLS})

  if (IS_ARM64)
    set(WN_ARCHITECTURE "ARM64")
  endif()
endif()

if (NOT DEFINED WN_ARCHITECTURE)
  message(FATAL_ERROR "Failed to determine architecture")
endif()

# determine archiutecture grouping and other properties where applicable
if (${WN_ARCHITECTURE} STREQUAL "x86-64")
  set(WN_ARCHITECTURE_GROUP "x86")
  set(WN_ARCHITECTURE_BITNESS 64)
  set(WN_ARCHITECTURE_ENDIANNESS "Little")
elseif (${WN_ARCHITECTURE} STREQUAL "x86")
  set(WN_ARCHITECTURE_GROUP "x86")
  set(WN_ARCHITECTURE_BITNESS 32)
  set(WN_ARCHITECTURE_ENDIANNESS "Little")
elseif (${WN_ARCHITECTURE} STREQUAL "ARM64")
  set(WN_ARCHITECTURE_GROUP "ARM")
  set(WN_ARCHITECTURE_BITNESS 64)
elseif (${WN_ARCHITECTURE} STREQUAL "ARM")
  set(WN_ARCHITECTURE_GROUP "ARM")
  set(WN_ARCHITECTURE_BITNESS 32)
endif()

# check architecture bitness if not already found
if (NOT DEFINED WN_ARCHITECTURE_BITNESS)
  check_type_size("void*" WN_ARCHITECTURE_BITNESS)
endif()

# check architecture endianness if not already found
if (NOT DEFINED WN_ARCHITECTURE_ENDIANNESS)
  test_big_endian(IS_BIG_ENDIAN)

  if (IS_BIG_ENDIAN)
    set(WN_ARCHITECTURE_ENDIANNESS "Big")
  else()
    set(WN_ARCHITECTURE_ENDIANNESS "Little")
  endif()
endif()

message(STATUS "Architecture: ${WN_ARCHITECTURE}")
message(STATUS "Architecture Bitness: ${WN_ARCHITECTURE_BITNESS}")
message(STATUS "Architecture Endianness: ${WN_ARCHITECTURE_ENDIANNESS}")