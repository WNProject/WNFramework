set(ENABLED OFF)
if (${CMAKE_GENERATOR} MATCHES "^Visual Studio.*")
set(ENABLED ON)
endif()

register_overlay(${ENABLED} MSBuild)