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