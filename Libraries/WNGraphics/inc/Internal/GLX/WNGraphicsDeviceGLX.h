// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_GLX_GRAPHICS_DEVICE_GLX_H__
#define __WN_GRAPHICS_GLX_GRAPHICS_DEVICE_GLX_H__

#include "WNGraphics/inc/WNGraphicsDevice.h"
#include "WNConcurrency/inc/WNRecursiveMutex.h"

#include <X11/Xlib.h>
#include <GL/glx.h>

namespace WNGraphics {
    class WNGraphicsDeviceGLX : public WNGraphicsDevice {
    public:
        WNGraphicsDeviceGLX(WNGraphics::WNGraphicsResourceFactory* _factory);

        virtual WNGraphicsDeviceReturnCode::type Initialize(wn_uint32 _adapter, wn_uint32 _output);
        virtual WN_FORCE_INLINE wn_void Release() {}

        // State Querying
        virtual wn_uint32 GetCapability(WNDeviceCaps _cap);
        virtual WNGraphicsDeviceReturnCode::type GetSize(wn_uint32 &_width, wn_uint32 &_height);
        virtual WNGraphicsDeviceReturnCode::type SetActiveSurface(wn::intrusive_ptr<wn::surface> _surface);

        // Resource Creation
        virtual WNShader* CreateShader(WNShaderTypes _type, wn_char* _shaderText);
        virtual WNBuffer* CreateBuffer(WNBufferTypes _type, wn_uint32 _elementSize, wn_uint32 _w, wn_uint32 _h, wn_uint32 _d);
        virtual WNTexture* CreateTexture(WNTextureTypes _type, WNTextureFormat _format, wn_uint32 _elementSize, wn_uint32 _w, wn_uint32 _h, wn_uint32 _d);
        virtual WNRenderTarget* CreateRenderTarget(WNTexture* _texture);
        virtual WNDrawList* CreateDrawList();

        // Resource Binding
        virtual WNGraphicsDeviceReturnCode::type BindShader(WNShader* _resource);
        virtual WNGraphicsDeviceReturnCode::type BindBuffer(WNBuffer* _resource, wn_uint32 _location);
        virtual WNGraphicsDeviceReturnCode::type BindTexture(WNTexture* _texture, wn_uint32 _location);
        virtual WNGraphicsDeviceReturnCode::type BindRenderTarget(WNRenderTarget* _texture, wn_uint32 _location);

        // State Setting
        virtual wn_void SetClearColor(wn_float32* _color);
        virtual WNGraphicsDeviceReturnCode::type BindSurface(wn::intrusive_ptr<wn::surface>& _surface, wn_bool _sync);

        // Drawing
        virtual WNGraphicsDeviceReturnCode::type SetDrawList(WNDrawList* _list);
        virtual WNGraphicsDeviceReturnCode::type StartDraw();
        virtual WNGraphicsDeviceReturnCode::type EndDraw();
        virtual WNGraphicsDeviceReturnCode::type SubmitDrawList(WNDrawList* _list);
        virtual WNGraphicsDeviceReturnCode::type Clear();
        


    private:
        enum WNInitializationState {
            eWNISUninitialized,
            eWNISDisplayOpened,
            eWNISFBConfigured,
            eWNISContextCreated,
            eWNISPBufferCreated,
            eWNISMax
        };

        struct __WNGraphicsData {
            wn_bool mSync;
        };

    private:
        wn::intrusive_ptr<wn::surface> mActiveSurface;
        WNGraphicsResourceFactory* mResourceFactory;
        Display* mDisplay;
        GLXContext  mContext;
        WNInitializationState mInitializationState;
        GLXPbuffer mTempPBuffer;
        XVisualInfo* mVisualInfo;
        GLXFBConfig mFBConfig;
    };
}

#endif // __WN_GRAPHICS_GLX_GRAPHICS_DEVICE_GLX_H__
