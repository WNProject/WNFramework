enable_overlay_directory()

add_wn_header_library(WNContainers
  SOURCES
    inc/WNArray.h
    inc/WNContiguousRange.h
    inc/WNDataBuffer.h
    inc/WNDeque.h
    inc/WNDynamicArray.h
    inc/WNFunction.h
    inc/WNHashMap.h
    inc/WNHashSet.h
    inc/WNList.h
    inc/WNPair.h
    inc/WNSerializer.h
    inc/WNSerializerBase.h
    inc/WNString.h
    inc/WNStringView.h
    inc/WNTuple.h
    inc/WNUniqueFunction.h
)

add_subdirectory(test)
