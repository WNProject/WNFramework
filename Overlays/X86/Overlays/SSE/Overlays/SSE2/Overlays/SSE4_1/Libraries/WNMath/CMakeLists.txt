set_overlay_var(WN_MATH_BASIC_TRAITS_H "WNMath/inc/Internal/WNSSE41BasicTraits.h")
set_overlay_var(WN_MATH_BASIC_TRAITS_TYPE "basic_traits_sse4_1")

set_overlay_var(WN_MATH_VECTOR_TRAITS_H
  "WNMath/inc/Internal/WNSSE41VectorTraits.h")
set_overlay_var(WN_MATH_VECTOR_TRAITS_TYPE "vector_traits_sse4_1")

overlay_header_library_sources(WNMath
  SOURCES
    inc/Internal/WNSSE41BasicTraits.h
    inc/Internal/WNSSE41VectorTraits.h
)
