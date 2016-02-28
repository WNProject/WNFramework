set(INC_ANDROID
  inc/Android/WNAndroidEventPump.h
  inc/Android/WNAppData.h
  inc/Android/WNLoggingData.h
)
set(SRC_ANDROID
    src/Android/WNAndroidEventPump.cpp
    src/Android/WNAppData.cpp
	src/Android/WNCrashHandler.cpp
	src/Android/WNLoggingData.cpp
)
set(SRC_POSIX
    src/Posix/WNCrashHandler.cpp
)
set(SRC_WINDOWS
    src/Windows/WNCrashHandler.cpp
)
set(WNUtilsSources
	inc/WNCrashHandler.h
	${INC_ANDROID}
)

if (WN_SYSTEM STREQUAL "Android")
  list(APPEND WNUtilsSources
    ${SRC_ANDROID})
  list(APPEND WNAdditionalUtilIncludes
    ${ANDROID_NDK}/sources/android/native_app_glue)
elseif (WN_POSIX)
  list(APPEND WNUtilsSources
    ${SRC_POSIX})
elseif (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WNUtilsSources
  ${SRC_WINDOWS} ${SRC_POSIX} ${SRC_ANDROID})
endif()
add_wn_library(WNUtils SOURCES
  ${WNUtilsSources}
)


if (WN_SYSTEM STREQUAL "Windows")
	set_source_files_properties(${SRC_POSIX} ${SRC_ANDROID} 
		PROPERTIES HEADER_FILE_ONLY true)
endif()

target_include_directories(WNUtils
  PUBLIC
  ${WNAdditionalUtilIncludes})


