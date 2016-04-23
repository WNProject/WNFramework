set_overlay_var(WN_MATH_BASIC_TRAITS_H "WNMath/inc/Internal/WNSSE2BasicTraits.h")
set_overlay_var(WN_MATH_BASIC_TRAITS_TYPE "basic_traits_sse2")

set_overlay_var(WN_MATH_ELEMENT_ARRAY_H
  "WNMath/inc/Internal/WNSSE2ElementArray.h")
set_overlay_var(WN_MATH_ELEMENT_ARRAY_TYPE "element_array_sse2")

set_overlay_var(WN_MATH_VECTOR_TRAITS_H
  "WNMath/inc/Internal/WNSSE2VectorTraits.h")
set_overlay_var(WN_MATH_VECTOR_TRAITS_TYPE "vector_traits_sse2")

overlay_header_library_sources(WNMath
  SOURCES
    inc/Internal/WNSSE2BasicTraits.h
    inc/Internal/WNSSE2ElementArray.h
    inc/Internal/WNSSE2VectorTraits.h
)
