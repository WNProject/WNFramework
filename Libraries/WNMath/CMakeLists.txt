option(WN_MATH_EXTENSIONS_DISABLED "Math Extensions are Enabled"
  ${WN_MATH_EXTENSIONS_DISABLED})

set(WN_MATH_BASIC_TRAITS_H "WNMath/inc/Internal/Generic/WNBasicTraits.h")
set(WN_MATH_BASIC_TRAITS_TYPE "basic_traits_generic")

set(WN_MATH_ELEMENT_ARRAY_H "WNMath/inc/Internal/Generic/WNElementArray.h")
set(WN_MATH_ELEMENT_ARRAY_TYPE "element_array_generic")

set(WN_MATH_VECTOR_TRAITS_H "WNMath/inc/Internal/Generic/WNVectorTraits.h")
set(WN_MATH_VECTOR_TRAITS_TYPE "vector_traits_generic")

set(WN_MATH_GENERAL_TRAITS_H "WNMath/inc/Internal/Generic/WNGeneralTraits.h")
set(WN_MATH_GENERAL_TRAITS_TYPE "general_traits_generic")

if (NOT WN_MATH_EXTENSIONS_DISABLED)
enable_overlay_directory()
endif()

if (WN_MATH_EXTENSIONS_DISABLED)
  set(WN_MATH_EXTENSIONS "_WN_MATH_EXTENSIONS_DISABLED")
else()
  set(WN_MATH_EXTENSIONS "_WN_MATH_EXTENSIONS_ENABLED")
endif()

configure_file(inc/Internal/WNConfig.h.in
  inc/Internal/WNConfig.h)
configure_file(inc/Internal/WNVectorTraits.h.in
  inc/Internal/WNVectorTraits.h)
configure_file(inc/Internal/WNElementArray.h.in
  inc/Internal/WNElementArray.h)
configure_file(inc/Internal/WNBasicTraits.h.in
  inc/Internal/WNBasicTraits.h)
configure_file(inc/Internal/WNGeneralTraits.h.in
  inc/Internal/WNGeneralTraits.h)
  
add_wn_header_library(WNMath
  SOURCES
    inc/WNBasic.h
    inc/WNCommon.h
    inc/WNConfig.h
    inc/WNConstants.h
    inc/WNVector.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNConfig.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNGeneralTraits.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNVectorTraits.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNElementArray.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNBasicTraits.h
    inc/Internal/WNBasicTraits.h.in
    inc/Internal/WNConfig.h.in
    inc/Internal/WNElementArray.h.in
    inc/Internal/WNGeneralTraits.h.in
    inc/Internal/WNVectorTraits.h.in
    inc/Internal/Generic/WNBasicTraits.h
    inc/Internal/Generic/WNElementArray.h
    inc/Internal/Generic/WNGeneralTraits.h
    inc/Internal/Generic/WNVectorTraits.h
)

wn_add_subdirectory(test)

post_enable_overlay_directory()