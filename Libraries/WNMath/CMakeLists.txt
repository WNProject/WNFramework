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
    inc/WNBounds2.h
    inc/WNBounds3.h
    inc/WNBox.h
    inc/WNCircle.h
    inc/WNCommon.h
    inc/WNConfig.h
    inc/WNConstants.h
    inc/WNFrustum.h
    inc/WNLine2.h
    inc/WNLine3.h
    inc/WNMath.h
    inc/WNMatrix2.h
    inc/WNMatrix3.h
    inc/WNMatrix4.h
    inc/WNMatrix.h
    inc/WNPlane.h
    inc/WNPoint.h
    inc/WNQuaternion.h
    inc/WNRay2.h
    inc/WNRay3.h
    inc/WNRectangle.h
    inc/WNSegment2.h
    inc/WNSegment3.h
    inc/WNSphere.h
    inc/WNVector.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNConfig.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNGeneralTraits.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNVectorTraits.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNElementArray.h
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNBasicTraits.h
    inc/Internal/WNBasicTraits.h.in
    inc/Internal/WNBounds2.inl
    inc/Internal/WNBox.inl
    inc/Internal/WNConfig.h.in
    inc/Internal/WNElementArray.h.in
    inc/Internal/WNFrustum.inl
    inc/Internal/WNGeneralTraits.h.in
    inc/Internal/WNLine2.inl
    inc/Internal/WNLine3.inl
    inc/Internal/WNMatrix2.inl
    inc/Internal/WNMatrix3Base.h
    inc/Internal/WNMatrix3Base.inl
    inc/Internal/WNMatrix3.inl
    inc/Internal/WNMatrix4Base.h
    inc/Internal/WNMatrix4Base.inl
    inc/Internal/WNMatrix4.inl
    inc/Internal/WNMatrixBase.h
    inc/Internal/WNMatrixBase.inl
    inc/Internal/WNMatrixX.inl
    inc/Internal/WNPlane.inl
    inc/Internal/WNQuaternionBase.h
    inc/Internal/WNQuaternionBase.inl
    inc/Internal/WNRay2.inl
    inc/Internal/WNRay3.inl
    inc/Internal/WNRectangle.inl
    inc/Internal/WNSegment2.inl
    inc/Internal/WNSegment3.inl
    inc/Internal/WNVectorTraits.h.in
    inc/Internal/Generic/WNBasicTraits.h
    inc/Internal/Generic/WNElementArray.h
    inc/Internal/Generic/WNGeneralTraits.h
    inc/Internal/Generic/WNVectorTraits.h
)

add_subdirectory(test)
