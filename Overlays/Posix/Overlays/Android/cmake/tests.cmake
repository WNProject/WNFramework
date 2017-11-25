
macro(overload_create_test)
if (NOT WN_ANDROID_WRAPPER)
  wn_create_test(${ARGN})
else()
  foreach(BUILD_ABI ${WN_SANITIZED_ABIS})
    wn_create_test(${ARGN} ABI ${BUILD_ABI})
  endforeach()
endif()
endmacro()

macro(overload_create_call_test)
  if (WN_ANDROID_WRAPPER)
    if (${prefix}_test_OVERLAY_SOURCES)
      foreach(source ${${prefix}_test_OVERLAY_SOURCES})
        get_filename_component(source ${source} NAME)
        foreach(BUILD_ABI ${WN_SANITIZED_ABIS})
            add_test(NAME ${prefix}_${source}_${BUILD_ABI}_test
              COMMAND
                  python ${WNFramework_BINARY_DIR}/${BUILD_ABI}/bin/${PARSED_ARGS_APPLICATION}.py
                  --install --run --stripped --remove --binary-dir ${WNFramework_BINARY_DIR}/${BUILD_ABI}/bin/
                  --copy-dir ${PARSED_ARGS_COMMAND_DIR}/
                  --tmp-dir /data/local/tmp/${prefix}_${source}_${BUILD_ABI}/
                  --args
                    "${PARSED_ARGS_COMMAND_PRE_DIR} /data/local/tmp/${prefix}_${source}_${BUILD_ABI} ${PARSED_ARGS_COMMAND_POST_DIR} ${source}"
              WORKING_DIRECTORY ${PARSED_ARGS_WORKING_DIRECTORY})
        endforeach()
      endforeach()
    else()
      foreach(BUILD_ABI ${WN_SANITIZED_ABIS})
        add_test(NAME ${prefix}_${BUILD_ABI}_test
              COMMAND
                  python ${WNFramework_BINARY_DIR}/${BUILD_ABI}/bin/${PARSED_ARGS_APPLICATION}.py
                  --install --run --stripped --remove --binary-dir ${WNFramework_BINARY_DIR}/${BUILD_ABI}/bin/
                  --copy-dir ${PARSED_ARGS_COMMAND_DIR}/
                  --tmp-dir /data/local/tmp/${prefix}_${BUILD_ABI}/
                  --args
                    "${PARSED_ARGS_COMMAND_PRE_DIR} /data/local/tmp/${prefix}_${BUILD_ABI} ${PARSED_ARGS_COMMAND_POST_DIR}"
        )
      endforeach()
    endif()
  endif()
endmacro()
