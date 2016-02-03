// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/EGL/WNGraphicsDeviceEGL.h"
#include "WNMultiTasking/inc/WNSpinLock.h"
#include "WNPlatform/inc/WNSurface.h"

#include <EGL/egl.h>
#include <GLES/gl.h>

static wn::multi_tasking::spin_lock gInitializationCreationLock;

WNGraphics::WNGraphicsDeviceEGL::WNGraphicsDeviceEGL(WNGraphics::WNGraphicsResourceFactory* _factory) :
    mResourceFactory(_factory),
    mDisplay(0) {
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceEGL::Initialize(wn_uint32 _adapter, wn_uint32 _output) {
    EGLConfig config;
    EGLint numConfigs;

    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE
    };

    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(mDisplay, 0, 0);
    eglChooseConfig(mDisplay, attribs, &config, 1, &numConfigs);
    mContext = eglCreateContext(mDisplay, config, NULL, NULL);
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

wn_uint32 WNGraphics::WNGraphicsDeviceEGL::GetCapability(WNDeviceCaps _cap) {
    switch(_cap) {
    case eWNVertexShaderCaps:
        return(0);
    case eWNPixelShaderCaps:
        return(0);
    case eWNGeometryShaderCaps:
        return(0);
    case eWNHullShaderCaps:
        return(0);
    case eWNDomainShaderCaps:
        return(0);
    case eWNComputeShaderCaps:
        return(0);
    case WNGraphics::eWNMultiDrawDevice:
        return(0);
    default:
        return(0);
    }
    return(0);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceEGL::GetSize(wn_uint32 &_width, wn_uint32 &_height) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNShader* WNGraphics::WNGraphicsDeviceEGL::CreateShader(WNGraphics::WNShaderTypes _type, wn_char* _shaderText) {
    return(wn_nullptr);
}

WNGraphics::WNBuffer* WNGraphics::WNGraphicsDeviceEGL::CreateBuffer(WNGraphics::WNBufferTypes _type, wn_uint32 _elementSize, wn_uint32 _w, wn_uint32 _h, wn_uint32 _d) {
    return(wn_nullptr);
}

WNGraphics::WNTexture* WNGraphics::WNGraphicsDeviceEGL::CreateTexture(WNGraphics::WNTextureTypes _type, WNGraphics::WNTextureFormat _format, wn_uint32 _elementSize, wn_uint32 _w, wn_uint32 _h, wn_uint32 _d) {
    return(wn_nullptr);
}

WNGraphics::WNRenderTarget* WNGraphics::WNGraphicsDeviceEGL::CreateRenderTarget(WNGraphics::WNTexture* _texture) {
    return(wn_nullptr);
}

WNGraphics::WNDrawList* WNGraphics::WNGraphicsDeviceEGL::CreateDrawList() {
    return(wn_nullptr);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceEGL::SetDrawList(WNDrawList* _list) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceEGL::StartDraw() {
    if (!mActiveSurface) {
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEBadSurface);
    } else {
        if (!eglMakeCurrent(mDisplay, mActiveSurface->GetNativeHandle(), mActiveSurface->GetNativeHandle(), mContext)) {
            return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEBadSurface);
        }
    }

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceEGL::EndDraw() {
    if (!mActiveSurface) {
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDENoSurface);
    }
    eglSwapBuffers(mDisplay, mActiveSurface->GetNativeHandle());
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceEGL::SubmitDrawList(WNGraphics::WNDrawList* _list) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceEGL::BindShader(WNGraphics::WNShader* _resource) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceEGL::BindBuffer(WNGraphics::WNBuffer* _resource, wn_uint32 _location) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceEGL::BindTexture(WNGraphics::WNTexture* _texture, wn_uint32 _location) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceEGL::BindRenderTarget(WNGraphics::WNRenderTarget* _texture, wn_uint32 _location) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceEGL::Clear() {
    glClear(GL_COLOR_BUFFER_BIT);
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceEGL::SetActiveSurface(wn::memory::intrusive_ptr<wn::surface> _surface) {
    mActiveSurface = _surface;
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

wn_void WNGraphics::WNGraphicsDeviceEGL::SetClearColor(wn_float32* _color) {
    glClearColor(_color[0], _color[1], _color[2], _color[3]);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceEGL::BindSurface(wn::memory::intrusive_ptr<wn::surface>& _surface, wn_bool _sync) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

