include_directories(${WNFramework_SOURCE_DIR}/Libraries)
include_directories(${WNFramework_BINARY_DIR}/Libraries)

foreach(overlay ${WN_ENABLED_OVERLAYS})
  include_directories(${overlay}/Libraries)
endforeach()

# Keep WNEntryPoint first. It has some special flags that have to propagate
# to other targets.
add_subdirectory(WNEntryPoint)
add_subdirectory(WNTesting)

add_subdirectory(WNContainers)
add_subdirectory(WNCore)
add_subdirectory(WNFileSystem)
add_subdirectory(WNFunctional)
add_subdirectory(WNLogging)
add_subdirectory(WNMath)
add_subdirectory(WNMemory)
add_subdirectory(WNMultiTasking)
add_subdirectory(WNNetworking)
add_subdirectory(WNScripting)
add_subdirectory(WNThreadSafeContainers)
add_subdirectory(WNUtilities)