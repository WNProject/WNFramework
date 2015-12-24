if (POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()

# Makes a C compliant preprocessor symbol
function(wn_make_preprocessor_symbol INPUT OUTPUT)
  string(MAKE_C_IDENTIFIER ${INPUT} SANITIZED_INPUT)
  string(TOUPPER ${SANITIZED_INPUT} SYMBOL)
  string(REGEX REPLACE "_+$" "" SYMBOL ${SYMBOL})

  set(${OUTPUT} ${SYMBOL} PARENT_SCOPE)
endfunction()

# determine architecture of target system. Output from this script are the
# following cmake variables
#
#   WN_ARCHITECTURE - stores the target architecture (x86-64, x86, ARM, ARM64,
#                     etc.)
#   WN_ARCHITECTURE_GROUP - stores the architecture group (x86, ARM, etc.). If
#                           there is no grouping available tehn cdefault to the
#                           value of WN_ARCHITECTURE
#   WN_ARCHITECTURE_BITNESS - Stores the bitness of the architecture as number
#                             of bits (32, 64, etc.)
#   WN_ARCHITECTURE_ENDIANNESS - Stores the endian of the architecture (Little
#                                or Big)
include(Utilities/CMake/determine_architecture.cmake)

# determines the platform of the target system. Output from this script are the
# following cmake variables
#
#   WN_PLATFORM - Stores the platfor name (Windows, Linux, Android, etc.)
include(Utilities/CMake/determine_platform.cmake)