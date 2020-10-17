
foreach(EXTERNAL_TARGET ${EXTERNAL_TARGETS})
    set_property(
      TARGET ${EXTERNAL_TARGET}
      PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endforeach()
