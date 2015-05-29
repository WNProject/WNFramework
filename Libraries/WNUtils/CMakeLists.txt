set(INC_ANDROID
  inc/Android/WNAndroidEventPump.h
  inc/Android/WNAppData.h
  inc/Android/WNLoggingData.h
)

set(WNUtilsSources
	inc/WNCrashHandler.h
	${INC_ANDROID}
)
source_group("inc\\Android" FILES ${INC_ANDROID})

if (WN_SYSTEM STREQUAL "Android")
  list(APPEND WNUtilsSources
    src/Android/WNAndroidEventPump.cpp
    src/Android/WNAppData.cpp
	src/Android/WNCrashHandler.cpp
	src/Android/WNLoggingData.cpp)
  list(APPEND WNAdditionalUtilIncludes
    ${ANDROID_NDK}/sources/android/native_app_glue)
elseif (WN_POSIX)
  list(APPEND WNUtilsSources
    src/Posix/WNCrashHandler.cpp)
elseif (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WNUtilsSources
    src/Windows/WNCrashHandler.cpp)
endif()
add_wn_library(WNUtils SOURCES
  ${WNUtilsSources}
)

target_include_directories(WNUtils
  PUBLIC
  ${CMAKE_SOURCE_DIR}/Libraries
  ${WNAdditionalUtilIncludes})


