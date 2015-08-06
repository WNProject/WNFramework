# Keep WNEntryPoint first. It has some special flags that have to propagate
# to other targets.
add_subdirectory(WNEntryPoint)

add_subdirectory(WNConcurrency)
add_subdirectory(WNContainers)
add_subdirectory(WNCore)
add_subdirectory(WNFileSystem)
add_subdirectory(WNGraphics)
add_subdirectory(WNLogging)
add_subdirectory(WNMath)
add_subdirectory(WNMemory)
add_subdirectory(WNNetworking)
add_subdirectory(WNScripting)
add_subdirectory(WNDeprecatedScripting)
add_subdirectory(WNStrings)
add_subdirectory(WNPlatform)
add_subdirectory(WNUtils)
