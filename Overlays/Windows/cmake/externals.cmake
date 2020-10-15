
foreach(TARGET ${EXTERNAL_TARGETS})
    set_property(
      TARGET ${tgt}
      PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endforeach()
