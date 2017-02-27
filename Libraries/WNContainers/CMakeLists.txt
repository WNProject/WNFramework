enable_overlay_directory()

add_wn_header_library(WNContainers
  SOURCES
    inc/WNArray.h
    inc/WNContiguousRange.h
    inc/WNDataBuffer.h
    inc/WNDeque.h
    inc/WNDynamicArray.h
    inc/WNHashMap.h
    inc/WNHashSet.h
    inc/WNRangePartition.h
    inc/WNList.h
    inc/WNSerializer.h
    inc/WNSerializerBase.h
    inc/WNString.h
    inc/WNStringView.h
)

wn_add_subdirectory(test)

post_enable_overlay_directory()