// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/EGL/WNGraphicsDeviceEGL.h"
#include "WNMultiTasking/inc/WNSpinLock.h"
#include "WNPlatform/inc/WNSurface.h"

#include <EGL/egl.h>
#include <GLES/gl.h>

static wn::multi_tasking::spin_lock gInitializationCreationLock;

WNGraphics::WNGraphicsDeviceEGL::WNGraphicsDeviceEGL(
    WNGraphics::WNGraphicsResourceFactory* _factory)
  : mResourceFactory(_factory), mDisplay(0) {}

WNGraphics::WNGraphicsDeviceReturnCode::type
WNGraphics::WNGraphicsDeviceEGL::Initialize(
    uint32_t _adapter, uint32_t _output) {
  EGLConfig config;
  EGLint numConfigs;

  const EGLint attribs[] = {EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_BLUE_SIZE, 8,
      EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_NONE};

  mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  eglInitialize(mDisplay, 0, 0);
  eglChooseConfig(mDisplay, attribs, &config, 1, &numConfigs);
  mContext = eglCreateContext(mDisplay, config, NULL, NULL);
  return (WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

uint32_t WNGraphics::WNGraphicsDeviceEGL::GetCapability(WNDeviceCaps _cap) {
  switch (_cap) {
    case eWNVertexShaderCaps:
      return (0);
    case eWNPixelShaderCaps:
      return (0);
    case eWNGeometryShaderCaps:
      return (0);
    case eWNHullShaderCaps:
      return (0);
    case eWNDomainShaderCaps:
      return (0);
    case eWNComputeShaderCaps:
      return (0);
    case WNGraphics::eWNMultiDrawDevice:
      return (0);
    default:
      return (0);
  }
  return (0);
}

WNGraphics::WNGraphicsDeviceReturnCode::type
WNGraphics::WNGraphicsDeviceEGL::GetSize(uint32_t& _width, uint32_t& _height) {
  return (WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNShader* WNGraphics::WNGraphicsDeviceEGL::CreateShader(
    WNGraphics::WNShaderTypes _type, char* _shaderText) {
  return (nullptr);
}

WNGraphics::WNBuffer* WNGraphics::WNGraphicsDeviceEGL::CreateBuffer(
    WNGraphics::WNBufferTypes _type, uint32_t _elementSize, uint32_t _w,
    uint32_t _h, uint32_t _d) {
  return (nullptr);
}

WNGraphics::WNTexture* WNGraphics::WNGraphicsDeviceEGL::CreateTexture(
    WNGraphics::WNTextureTypes _type, WNGraphics::WNTextureFormat _format,
    uint32_t _elementSize, uint32_t _w, uint32_t _h, uint32_t _d) {
  return (nullptr);
}

WNGraphics::WNRenderTarget* WNGraphics::WNGraphicsDeviceEGL::CreateRenderTarget(
    WNGraphics::WNTexture* _texture) {
  return (nullptr);
}

WNGraphics::WNDrawList* WNGraphics::WNGraphicsDeviceEGL::CreateDrawList() {
  return (nullptr);
}

WNGraphics::WNGraphicsDeviceReturnCode::type
WNGraphics::WNGraphicsDeviceEGL::SetDrawList(WNDrawList* _list) {
  return (WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type
WNGraphics::WNGraphicsDeviceEGL::StartDraw() {
  if (!mActiveSurface) {
    return (WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEBadSurface);
  } else {
    if (!eglMakeCurrent(mDisplay, mActiveSurface->GetNativeHandle(),
            mActiveSurface->GetNativeHandle(), mContext)) {
      return (WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEBadSurface);
    }
  }

  return (WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type
WNGraphics::WNGraphicsDeviceEGL::EndDraw() {
  if (!mActiveSurface) {
    return (WNGraphics::WNGraphicsDeviceReturnCode::eWNGDENoSurface);
  }
  eglSwapBuffers(mDisplay, mActiveSurface->GetNativeHandle());
  return (WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type
WNGraphics::WNGraphicsDeviceEGL::SubmitDrawList(WNGraphics::WNDrawList* _list) {
  return (WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type
WNGraphics::WNGraphicsDeviceEGL::BindShader(WNGraphics::WNShader* _resource) {
  return (WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type
WNGraphics::WNGraphicsDeviceEGL::BindBuffer(
    WNGraphics::WNBuffer* _resource, uint32_t _location) {
  return (WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type
WNGraphics::WNGraphicsDeviceEGL::BindTexture(
    WNGraphics::WNTexture* _texture, uint32_t _location) {
  return (WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type
WNGraphics::WNGraphicsDeviceEGL::BindRenderTarget(
    WNGraphics::WNRenderTarget* _texture, uint32_t _location) {
  return (WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type
WNGraphics::WNGraphicsDeviceEGL::Clear() {
  glClear(GL_COLOR_BUFFER_BIT);
  return (WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type
WNGraphics::WNGraphicsDeviceEGL::SetActiveSurface(
    wn::memory::intrusive_ptr<wn::surface> _surface) {
  mActiveSurface = _surface;
  return (WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

void WNGraphics::WNGraphicsDeviceEGL::SetClearColor(float* _color) {
  glClearColor(_color[0], _color[1], _color[2], _color[3]);
}

WNGraphics::WNGraphicsDeviceReturnCode::type
WNGraphics::WNGraphicsDeviceEGL::BindSurface(
    wn::memory::intrusive_ptr<wn::surface>& _surface, bool _sync) {
  return (WNGraphics::WNGraphicsDeviceReturnCode::ok);
}
