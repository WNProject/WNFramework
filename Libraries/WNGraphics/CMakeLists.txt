set(INC_INTERNAL_D3D11
  inc/Internal/D3D11/WNGraphicsDeviceD3D11.h
)
set(SRC_INTERNAL_D3D11
	src/D3D11/WNGraphicsDeviceD3D11.cpp)
set(INC_INTERNAL_GLX
  inc/Internal/GLX/WNGraphicsDeviceGLX.h
)
set(SRC_INTERNAL_GLX
    src/GLX/WNGraphicsDeviceGLX.cpp)
set(INC_INTERNAL_EGL
  inc/Internal/EGL/WNGraphicsDeviceEGL.h
)
set(SRC_INTERNAL_EGL
  src/EGL/WNGraphicsDeviceEGL.cpp)

set(WNGraphicsSources
  inc/WNBaseTexture.h
  inc/WNBuffer.h
  inc/WNDrawList.h
  inc/WNGraphicsBuffer.h
  inc/WNGraphicsDeviceFactory.h
  inc/WNGraphicsDevice.h
  inc/WNGraphics.h
  inc/WNGraphicsResourceFactory.h
  inc/WNGraphicsResource.h
  inc/WNRenderTarget.h
  inc/WNShader.h
  inc/WNTexture.h
  inc/WNVertexLayout.h
  src/WNGraphicsDeviceFactory.cpp
  ${INC_INTERNAL}
  ${INC_INTERNAL_D3D11}
  ${INC_INTERNAL_EGL}
  ${INC_INTERNAL_GLX})

set(WNGraphicsLinkLibraries
  "")

if (WN_SYSTEM STREQUAL "Android")
  list(APPEND WNGraphicsSources
    ${SRC_INTERNAL_EGL})
  list(APPEND WNGraphicsLinkLibraries GLESv2)
elseif (WN_SYSTEM STREQUAL "Linux")
  list(APPEND WNGraphicsSources
    ${SRC_INTERNAL_GLX})
  list(APPEND WNGraphicsLinkLibraries
    X11 GL)
elseif (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WNGraphicsSources
    ${SRC_INTERNAL_D3D11} ${SRC_INTERNAL_EGL}
	${SRC_INTERNAL_GLX})
endif()

add_wn_library(WNGraphics SOURCES
  ${WNGraphicsSources}
  )
 
source_group("inc\\Internal\\D3D11" FILES ${INC_INTERNAL_D3D11})
source_group("inc\\Internal\\GLX" FILES ${INC_INTERNAL_GLX})
source_group("inc\\Internal\\EGL" FILES ${INC_INTERNAL_EGL})

if (WN_SYSTEM STREQUAL "Windows")
  source_group("src\\D3D11" FILES ${SRC_INTERNAL_D3D11})
  source_group("src\\GLX" FILES ${SRC_INTERNAL_GLX})
  source_group("src\\EGL" FILES ${SRC_INTERNAL_EGL})
  set_source_files_properties(${SRC_INTERNAL_GLX} ${SRC_INTERNAL_EGL}
    PROPERTIES HEADER_FILE_ONLY true)
endif()

wn_target_link_libraries(WNGraphics PUBLIC WNPlatform ${WNGraphicsLinkLibraries})
target_include_directories(WNGraphics PUBLIC
  ${CMAKE_SOURCE_DIR}/Libraries)

add_subdirectory(test)
