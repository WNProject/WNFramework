// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/Internal/EGL/WNGraphicsDeviceEGL.h"
#include "WNConcurrency/inc/WNSpinLock.h"
#include "WNConcurrency/inc/WNLockGuard.h"
#include "WNPlatform/inc/WNSurface.h"

#include <EGL/egl.h>
#include <GLES/gl.h>
static WNConcurrency::WNSpinLock gInitializationCreationLock;

WNGraphics::WNGraphicsDeviceEGL::WNGraphicsDeviceEGL(WNGraphics::WNGraphicsResourceFactory* _factory) :
    mResourceFactory(_factory),
    mDisplay(0) {
}

WNGraphics::WNGraphicsDeviceReturnCode::Type WNGraphics::WNGraphicsDeviceEGL::Initialize(WN_UINT32 _adapter, WN_UINT32 _output) {
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
    return(WNGraphics::WNGraphicsDeviceReturnCode::eWNOK);
}

WN_UINT32 WNGraphics::WNGraphicsDeviceEGL::GetCapability(WNDeviceCaps _cap) {
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

WNGraphics::WNGraphicsDeviceReturnCode::Type WNGraphics::WNGraphicsDeviceEGL::GetSize(WN_UINT32 &_width, WN_UINT32 &_height) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::eWNOK);
}

WNGraphics::WNShader* WNGraphics::WNGraphicsDeviceEGL::CreateShader(WNGraphics::WNShaderTypes _type, WN_CHAR* _shaderText) {
    return(WN_NULL);
}

WNGraphics::WNBuffer* WNGraphics::WNGraphicsDeviceEGL::CreateBuffer(WNGraphics::WNBufferTypes _type, WN_UINT32 _elementSize, WN_UINT32 _w, WN_UINT32 _h, WN_UINT32 _d) {
    return(WN_NULL);
}

WNGraphics::WNTexture* WNGraphics::WNGraphicsDeviceEGL::CreateTexture(WNGraphics::WNTextureTypes _type, WNGraphics::WNTextureFormat _format, WN_UINT32 _elementSize, WN_UINT32 _w, WN_UINT32 _h, WN_UINT32 _d) {
    return(WN_NULL);
}

WNGraphics::WNRenderTarget* WNGraphics::WNGraphicsDeviceEGL::CreateRenderTarget(WNGraphics::WNTexture* _texture) {
    return(WN_NULL);
}

WNGraphics::WNDrawList* WNGraphics::WNGraphicsDeviceEGL::CreateDrawList() {
    return(WN_NULL);
}

WNGraphics::WNGraphicsDeviceReturnCode::Type WNGraphics::WNGraphicsDeviceEGL::SetDrawList(WNDrawList* _list) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::eWNOK);
}

WNGraphics::WNGraphicsDeviceReturnCode::Type WNGraphics::WNGraphicsDeviceEGL::StartDraw() {
    if (!mActiveSurface) {
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEBadSurface);
    } else {
        if (!eglMakeCurrent(mDisplay, mActiveSurface->GetNativeHandle(), mActiveSurface->GetNativeHandle(), mContext)) {
            return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEBadSurface);
        }
    }

    return(WNGraphics::WNGraphicsDeviceReturnCode::eWNOK);
}

WNGraphics::WNGraphicsDeviceReturnCode::Type WNGraphics::WNGraphicsDeviceEGL::EndDraw() {
    if (!mActiveSurface) {
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDENoSurface);
    }
    eglSwapBuffers(mDisplay, mActiveSurface->GetNativeHandle());
    return(WNGraphics::WNGraphicsDeviceReturnCode::eWNOK);
}

WNGraphics::WNGraphicsDeviceReturnCode::Type WNGraphics::WNGraphicsDeviceEGL::SubmitDrawList(WNGraphics::WNDrawList* _list) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::eWNOK);
}

WNGraphics::WNGraphicsDeviceReturnCode::Type WNGraphics::WNGraphicsDeviceEGL::BindShader(WNGraphics::WNShader* _resource) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::eWNOK);
}

WNGraphics::WNGraphicsDeviceReturnCode::Type WNGraphics::WNGraphicsDeviceEGL::BindBuffer(WNGraphics::WNBuffer* _resource, WN_UINT32 _location) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::eWNOK);
}

WNGraphics::WNGraphicsDeviceReturnCode::Type WNGraphics::WNGraphicsDeviceEGL::BindTexture(WNGraphics::WNTexture* _texture, WN_UINT32 _location) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::eWNOK);
}

WNGraphics::WNGraphicsDeviceReturnCode::Type WNGraphics::WNGraphicsDeviceEGL::BindRenderTarget(WNGraphics::WNRenderTarget* _texture, WN_UINT32 _location) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::eWNOK);
}

WNGraphics::WNGraphicsDeviceReturnCode::Type WNGraphics::WNGraphicsDeviceEGL::Clear() {
    glClear(GL_COLOR_BUFFER_BIT);
    return(WNGraphics::WNGraphicsDeviceReturnCode::eWNOK);
}

WNGraphics::WNGraphicsDeviceReturnCode::Type WNGraphics::WNGraphicsDeviceEGL::SetActiveSurface(WNConcurrency::WNResourcePointer<WNPlatform::WNSurface> _surface) {
    mActiveSurface = _surface;
    return(WNGraphics::WNGraphicsDeviceReturnCode::eWNOK);
}

WN_VOID WNGraphics::WNGraphicsDeviceEGL::SetClearColor(WN_FLOAT32* _color) {
    glClearColor(_color[0], _color[1], _color[2], _color[3]);
}

WNGraphics::WNGraphicsDeviceReturnCode::Type WNGraphics::WNGraphicsDeviceEGL::BindSurface(WNConcurrency::WNResourcePointer<WNPlatform::WNSurface>& _surface, WN_BOOL _sync) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::eWNOK);
}

