// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/Internal/GLX/WNGraphicsDeviceGLX.h"
#include "WNConcurrency/inc/WNSpinLock.h"
#include "WNPlatform/inc/WNSurface.h"

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
typedef Bool (*glXMakeContextCurrentARBProc)(Display*, GLXDrawable, GLXDrawable, GLXContext);
typedef void (*glXSwapIntervalEXTProc)(Display *dpy, GLXDrawable drawable, int interval);

static glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
static glXMakeContextCurrentARBProc glXMakeContextCurrentARB = 0;
static glXSwapIntervalEXTProc glXSwapIntervalEXT = 0;

#define WN_RESIZE_RETRY_COUNT 5

static WNConcurrency::WNSpinLock gInitializationCreationLock;

WNGraphics::WNGraphicsDeviceGLX::WNGraphicsDeviceGLX(WNGraphics::WNGraphicsResourceFactory* _factory) :
    mResourceFactory(_factory),
    mDisplay(0) {
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceGLX::Initialize(wn_uint32 _adapter, wn_uint32 _output) {
    static int visual_attribs[] =
    {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
//TODO: Try to create with samples, and fall back to none
//        GLX_SAMPLE_BUFFERS, 1,
//        GLX_SAMPLES, 1,
        None
    };

    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 2,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        None
    };

    mDisplay = XOpenDisplay(0);

    if (!mDisplay) {
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEWindowCreationError);
    }

    mInitializationState = WNGraphics::WNGraphicsDeviceGLX::eWNISDisplayOpened;

    int fbcount = 0;

    GLXFBConfig* fbc = glXChooseFBConfig(mDisplay, DefaultScreen(mDisplay), visual_attribs, &fbcount);

    if (!fbc) {
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEDisplayModeError);
    }

    mFBConfig = fbc[0];

    XFree(fbc);

    mInitializationState = WNGraphics::WNGraphicsDeviceGLX::eWNISFBConfigured;

    gInitializationCreationLock.Lock();

    if (!glXCreateContextAttribsARB) {
        glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB");
        glXMakeContextCurrentARB = (glXMakeContextCurrentARBProc)glXGetProcAddressARB( (const GLubyte *) "glXMakeContextCurrent");
        glXSwapIntervalEXT = (glXSwapIntervalEXTProc)glXGetProcAddressARB( (const GLubyte *) "glXSwapIntervalEXT");
    }

    gInitializationCreationLock.Unlock();

    if (!(glXCreateContextAttribsARB && glXMakeContextCurrentARB)) {
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEWindowCreationError);
    }

    if(!(mContext = glXCreateContextAttribsARB(mDisplay, mFBConfig, 0, true, context_attribs))){
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEWindowCreationError);
    }

    mInitializationState = WNGraphics::WNGraphicsDeviceGLX::eWNISContextCreated;

    static int pbuffer_attribs[] = {
            GLX_PBUFFER_WIDTH, 800,
            GLX_PBUFFER_HEIGHT, 600,
            None
    };

    mTempPBuffer = glXCreatePbuffer(mDisplay, mFBConfig, pbuffer_attribs);

    if (!glXMakeContextCurrent(mDisplay, mTempPBuffer, mTempPBuffer, mContext)){
        if (!glXMakeContextCurrent(mDisplay, DefaultRootWindow(mDisplay), DefaultRootWindow(mDisplay), mContext)) {
            return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEMonitorError);
        }
    }

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

wn_uint32 WNGraphics::WNGraphicsDeviceGLX::GetCapability(WNDeviceCaps _cap) {
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

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceGLX::GetSize(wn_uint32 &_width, wn_uint32 &_height) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNShader* WNGraphics::WNGraphicsDeviceGLX::CreateShader(WNGraphics::WNShaderTypes _type, wn_char* _shaderText) {
    return(wn_nullptr);
}

WNGraphics::WNBuffer* WNGraphics::WNGraphicsDeviceGLX::CreateBuffer(WNGraphics::WNBufferTypes _type, wn_uint32 _elementSize, wn_uint32 _w, wn_uint32 _h, wn_uint32 _d) {
    return(wn_nullptr);
}

WNGraphics::WNTexture* WNGraphics::WNGraphicsDeviceGLX::CreateTexture(WNGraphics::WNTextureTypes _type, WNGraphics::WNTextureFormat _format, wn_uint32 _elementSize, wn_uint32 _w, wn_uint32 _h, wn_uint32 _d) {
    return(wn_nullptr);
}

WNGraphics::WNRenderTarget* WNGraphics::WNGraphicsDeviceGLX::CreateRenderTarget(WNGraphics::WNTexture* _texture) {
    return(wn_nullptr);
}

WNGraphics::WNDrawList* WNGraphics::WNGraphicsDeviceGLX::CreateDrawList() {
    return(wn_nullptr);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceGLX::SetDrawList(WNDrawList* _list) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceGLX::StartDraw() {
    if (!mActiveSurface) {
        if (!glXMakeContextCurrent(mDisplay, mTempPBuffer, mTempPBuffer, mContext)){
            if (!glXMakeContextCurrent(mDisplay, DefaultRootWindow(mDisplay), DefaultRootWindow(mDisplay), mContext)) {
                return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEMonitorError);
            }
        }
    } else {
        if (!glXMakeContextCurrent(mDisplay, mActiveSurface->GetNativeHandle(), mActiveSurface->GetNativeHandle(), mContext)) {
            return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEBadSurface);
        }
    }

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceGLX::EndDraw() {
    if (!mActiveSurface) {
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDENoSurface);
    }

    glXSwapBuffers ( mDisplay, mActiveSurface->GetNativeHandle() );
    glFinish();

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceGLX::SubmitDrawList(WNGraphics::WNDrawList* _list) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceGLX::BindShader(WNGraphics::WNShader* _resource) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceGLX::BindBuffer(WNGraphics::WNBuffer* _resource, wn_uint32 _location) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceGLX::BindTexture(WNGraphics::WNTexture* _texture, wn_uint32 _location) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceGLX::BindRenderTarget(WNGraphics::WNRenderTarget* _texture, wn_uint32 _location) {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceGLX::Clear() {
    glClear(GL_COLOR_BUFFER_BIT);

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceGLX::SetActiveSurface(wn::memory::intrusive_ptr<wn::surface> _surface) {
    mActiveSurface = _surface;

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

wn_void WNGraphics::WNGraphicsDeviceGLX::SetClearColor(wn_float32* _color) {
    glClearColor(_color[0], _color[1], _color[2], _color[3]);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceGLX::BindSurface(wn::memory::intrusive_ptr<wn::surface>& _surface, wn_bool _sync) {
    __WNGraphicsData * data = WN_NEW __WNGraphicsData();

    XSync(mDisplay, false);

    data->mSync = _sync;

    if (data->mSync && glXSwapIntervalEXT) {
        unsigned int maxSwap;

        glXQueryDrawable(mDisplay, _surface->GetNativeHandle(), GLX_MAX_SWAP_INTERVAL_EXT, &maxSwap);
        glXSwapIntervalEXT(mDisplay, _surface->GetNativeHandle(), maxSwap);
    }

    _surface->RegisterData(wn::surface::eWNRDTGraphics, data);

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}
