set(INC_INTERNAL
  inc/Internal/WNCallback.inl
  inc/Internal/WNCallbackBase.h
  inc/Internal/WNCallbackBase.inl
)
source_group("inc\\Internal" FILES ${INC_INTERNAL})
add_wn_header_library(WNContainers
  inc/WNCallback.h
  inc/WNContainers.h
  inc/WNDataBuffer.h
  inc/WNDeque.h
  inc/WNDynamicArray.h
  inc/WNSerializer.h
  inc/WNSerializerBase.h
  ${INC_INTERNAL}
)

add_subdirectory(test)
