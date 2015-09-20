set(NATVIS
  natvis/WNContiguousRange.natvis
  natvis/WNStringView.natvis
)

source_group("natvis" FILES ${NATVIS})

add_wn_header_library(WNContainers
  ${NATVIS}
  inc/WNContiguousRange.h
  inc/WNDataBuffer.h
  inc/WNDeque.h
  inc/WNDynamicArray.h
  inc/WNFunction.h
  inc/WNHashMap.h
  inc/WNHashSet.h
  inc/WNList.h
  inc/WNSerializer.h
  inc/WNSerializerBase.h
  inc/WNString.h
  inc/WNStringView.h
)

add_subdirectory(test)