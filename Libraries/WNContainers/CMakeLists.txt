set(WN_CONTAINERS_FILES)

if (MSVC)
  set(NATVIS_FILES
    natvis/WNContiguousRange.natvis
    natvis/WNPair.natvis
    natvis/WNStringView.natvis
  )

  list(APPEND WN_CONTAINERS_FILES ${NATVIS_FILES})

  source_group("natvis" FILES ${NATVIS_FILES})
endif()

add_wn_header_library(WNContainers
  ${WN_CONTAINERS_FILES}
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
)

add_subdirectory(test)