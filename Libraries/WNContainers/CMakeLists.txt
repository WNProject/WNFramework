set(WN_CONTAINERS_FILES)

if (WN_SYSTEM STREQUAL "Windows")
  set(NATVIS_FILES
    natvis/WNContiguousRange.natvis
    natvis/WNStringView.natvis
  )

  list(APPEND WN_CONTAINERS_FILES ${NATVIS_FILES})

  source_group("natvis" FILES ${NATVIS_FILES})
endif()

add_wn_header_library(WNContainers
  ${WN_CONTAINERS_FILES}
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