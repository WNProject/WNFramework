set(INC_INTERNAL_D3D11
  inc/Internal/D3D11/WNGraphicsDeviceD3D11.h
)
set(INC_INTERNAL_GLX
  inc/Interal/GLX/WNGraphicsDeviceGLX.h
)
set(INC_INTERNAL_EGL
  inc/Internal/EGL/WNGraphicsDeviceEGL.h
)
source_group("inc\\Internal\\D3D11" FILES ${INC_INTERNAL_D3D11})
source_group("inc\\Internal\\GLX" FILES ${INC_INTERNAL_GLX})
source_group("inc\\Internal\\EGL" FILES ${INC_INTERNAL_EGL})

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
  ${INC_INTERNAL_ANDROID}
  ${INC_INTERNAL_WINDOWS}
  ${INC_INTERNAL_XWINDOWS})

set(WNGraphicsLinkLibraries
  "")

if (WN_SYSTEM STREQUAL "Android")
  list(APPEND WNGraphicsSources
    src/EGL/WNGraphicsDeviceEGL.cpp)
elseif (WN_SYSTEM STREQUAL "Linux")
  list(APPEND WNGraphicsSources
    src/GLX/WNGraphicsDeviceGLX.cpp)
  list(APPEND WNGraphicsLinkLibraries
    X11 GL)
elseif (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WNGraphicsSources
    src/D3D11/WNGraphicsDeviceD3D11.cpp)
endif()

add_wn_library(WNGraphics STATIC
  ${WNGraphicsSources}
  )

target_link_libraries(WNGraphics PUBLIC WNPlatform ${WNGraphicsLinkLibraries})
target_include_directories(WNGraphics PUBLIC
  ${CMAKE_SOURCE_DIR}/Libraries)

add_subdirectory(test)
