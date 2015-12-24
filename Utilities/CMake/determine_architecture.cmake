include(CheckTypeSize)
include(CheckSymbolExists)
include(TestBigEndian)

function(wn_check_any_symbol_exists LIST_VARIABLE_NAME VARIABLE)
  foreach(SYMBOL ${${LIST_VARIABLE_NAME}})
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

# check system architecture using cmake system variables
if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
  # cmake always reports AMD64 for msvc, need to check for bit width
  if (MSVC AND NOT CMAKE_CL_64)
    set(WN_ARCHITECTURE "x86")
  else()
    set(WN_ARCHITECTURE "x86-64")
  endif()
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86|i386|i686)$")
  set(WN_ARCHITECTURE "x86")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64.*|AARCH64.*)")
  set(WN_ARCHITECTURE "ARM64")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm.*|ARM.*)")
  set(WN_ARCHITECTURE "ARM")
else()
  # list of all suported architectures
  set(ARCHITECTURE_LIST "x86-64" x86 ARM64 ARM)

  # architectural preprocessor symbols x86-64
  set(X86_64_SYMBOLS
    __amd64__
    __amd64
    __x86_64__
    __x86_64
    _M_X64
    _M_AMD64
  )

  # architectural preprocessor symbols x86
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

  # architectural preprocessor symbols ARM64
  set(ARM64_SYMBOLS
    __aarch64__
  )

  # architectural preprocessor symbols ARM
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

  # check for architectures using preprocessor symbols. Symbol list names must
  # match the format [architecture, all caps, alphanumeric and _ only]_SYMBOLS.
  foreach(ARCHITECTURE ${ARCHITECTURE_LIST})
    wn_make_preprocessor_symbol(${ARCHITECTURE} ARCHITECTURE)

    set(ARCHITECTURE_SYMBOL_LIST_NAME ${ARCHITECTURE}_SYMBOLS)
    set(ARCHITECTURE_HAS_VARIABLE_NAME HAS_${ARCHITECTURE}_SYMBOL)

    wn_check_any_symbol_exists(
      ${ARCHITECTURE_SYMBOL_LIST_NAME}
      ${ARCHITECTURE_HAS_VARIABLE_NAME}
    )

    if (${ARCHITECTURE_HAS_VARIABLE_NAME})
      set(WN_ARCHITECTURE ${ARCHITECTURE})
      break()
    endif()
  endforeach()
endif()

if (NOT DEFINED WN_ARCHITECTURE)
  message(FATAL_ERROR "Failed to determine architecture")
endif()

# determine archiutecture grouping and other properties where applicable
if (WN_ARCHITECTURE STREQUAL "x86-64")
  set(WN_ARCHITECTURE_GROUP "x86")
  set(WN_ARCHITECTURE_BITNESS 64)
  set(WN_ARCHITECTURE_ENDIANNESS "Little")
elseif (WN_ARCHITECTURE STREQUAL "x86")
  set(WN_ARCHITECTURE_GROUP "x86")
  set(WN_ARCHITECTURE_BITNESS 32)
  set(WN_ARCHITECTURE_ENDIANNESS "Little")
elseif (WN_ARCHITECTURE STREQUAL "ARM64")
  set(WN_ARCHITECTURE_GROUP "ARM")
  set(WN_ARCHITECTURE_BITNESS 64)
elseif (WN_ARCHITECTURE STREQUAL "ARM")
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

if (NOT DEFINED WN_ARCHITECTURE_GROUP)
  set(WN_ARCHITECTURE_GROUP ${WN_ARCHITECTURE})
endif()

message(STATUS "Architecture: ${WN_ARCHITECTURE}")
message(STATUS "Architecture Group: ${WN_ARCHITECTURE_GROUP}")
message(STATUS "Architecture Bitness: ${WN_ARCHITECTURE_BITNESS}")
message(STATUS "Architecture Endianness: ${WN_ARCHITECTURE_ENDIANNESS}")