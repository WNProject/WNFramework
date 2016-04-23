set_overlay_var(WN_MATH_BASIC_TRAITS_H "WNMath/inc/Internal/WNSSEBasicTraits.h")
set_overlay_var(WN_MATH_BASIC_TRAITS_TYPE "basic_traits_sse")

set_overlay_var(WN_MATH_ELEMENT_ARRAY_H
  "WNMath/inc/Internal/WNSSEElementArray.h")
set_overlay_var(WN_MATH_ELEMENT_ARRAY_TYPE "element_array_sse")

set_overlay_var(WN_MATH_VECTOR_TRAITS_H
  "WNMath/inc/Internal/WNSSEVectorTraits.h")
set_overlay_var(WN_MATH_VECTOR_TRAITS_TYPE "vector_traits_sse")

set_overlay_var(WN_MATH_GENERAL_TRAITS_H
  "WNMath/inc/Internal/WNSSEGeneralTraits.h")
set_overlay_var(WN_MATH_GENERAL_TRAITS_TYPE "general_traits_sse")

overlay_header_library_sources(WNMath
  SOURCES
    inc/Internal/WNSSEBasicTraits.h
    inc/Internal/WNSSEElementArray.h
    inc/Internal/WNSSEGeneralTraits.h
    inc/Internal/WNSSEVectorTraits.h
)
