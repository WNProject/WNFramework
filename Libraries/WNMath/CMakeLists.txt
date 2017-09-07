set(WN_MATH_BASIC_TRAITS_H "WNMath/inc/Internal/Generic/WNBasicTraits.h")
set(WN_MATH_BASIC_TRAITS_TYPE "basic_traits_generic")

set(WN_MATH_COMMON_BASE_H "WNMath/inc/Internal/Generic/WNCommonBase.h")
set(WN_MATH_COMMON_BASE_TYPE "common_base_generic")

set(WN_MATH_MATRIX_BASE_H "WNMath/inc/Internal/Generic/WNMatrixBase.h")
set(WN_MATH_MATRIX_BASE_TYPE "matrix_base_generic")

set(WN_MATH_STORAGE_BASE_H "WNMath/inc/Internal/Generic/WNStorageBase.h")
set(WN_MATH_STORAGE_BASE_TYPE "storage_base_generic")

set(WN_MATH_VECTOR_BASE_H "WNMath/inc/Internal/Generic/WNVectorBase.h")
set(WN_MATH_VECTOR_BASE_TYPE "vector_base_generic")

if (NOT WN_MATH_EXTENSIONS_DISABLED)
enable_overlay_directory()
endif()

configure_file(inc/Internal/WNBasicTraits.h.in
  inc/Internal/WNBasicTraits.h)
configure_file(inc/Internal/WNCommonBase.h.in
  inc/Internal/WNCommonBase.h)
configure_file(inc/Internal/WNMatrixBase.h.in
  inc/Internal/WNMatrixBase.h)
configure_file(inc/Internal/WNStorageBase.h.in
  inc/Internal/WNStorageBase.h)
configure_file(inc/Internal/WNVectorBase.h.in
  inc/Internal/WNVectorBase.h)

add_wn_header_library(WNMath
  SOURCES
    inc/WNBasic.h
    inc/WNCommon.h
    inc/WNConstants.h
    inc/WNMatrix.h
    inc/WNVector.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNBasicTraits.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNCommonBase.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNMatrixBase.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNStorageBase.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNVectorBase.h
    inc/Internal/WNBasicTraits.h.in
    inc/Internal/WNCommonBase.h.in
    inc/Internal/WNMatrixBase.h.in
    inc/Internal/WNStorageBase.h.in
    inc/Internal/WNVectorBase.h.in
    inc/Internal/Generic/WNBasicTraits.h
    inc/Internal/Generic/WNCommonBase.h
    inc/Internal/Generic/WNMatrixBase.h
    inc/Internal/Generic/WNStorageBase.h
    inc/Internal/Generic/WNVectorBase.h
)

wn_add_subdirectory(test)

post_enable_overlay_directory()
