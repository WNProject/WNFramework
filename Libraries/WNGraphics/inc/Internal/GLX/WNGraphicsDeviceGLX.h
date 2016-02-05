// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_GLX_GRAPHICS_DEVICE_GLX_H__
#define __WN_GRAPHICS_GLX_GRAPHICS_DEVICE_GLX_H__

#include "WNGraphics/inc/WNGraphicsDevice.h"
#include "WNMultiTasking/inc/WNMutex.h"
#include "WNPlatform/inc/WNSurface.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

#include <X11/Xlib.h>
#include <GL/glx.h>

namespace WNGraphics {
    class WNGraphicsDeviceGLX : public WNGraphicsDevice {
    public:
        WNGraphicsDeviceGLX(WNGraphics::WNGraphicsResourceFactory* _factory);

        virtual WNGraphicsDeviceReturnCode::type Initialize(uint32_t _adapter, uint32_t _output);
        virtual WN_FORCE_INLINE void Release() {}

        // State Querying
        virtual uint32_t GetCapability(WNDeviceCaps _cap);
        virtual WNGraphicsDeviceReturnCode::type GetSize(uint32_t &_width, uint32_t &_height);
        virtual WNGraphicsDeviceReturnCode::type SetActiveSurface(wn::memory::intrusive_ptr<wn::surface> _surface);

        // Resource Creation
        virtual WNShader* CreateShader(WNShaderTypes _type, char* _shaderText);
        virtual WNBuffer* CreateBuffer(WNBufferTypes _type, uint32_t _elementSize, uint32_t _w, uint32_t _h, uint32_t _d);
        virtual WNTexture* CreateTexture(WNTextureTypes _type, WNTextureFormat _format, uint32_t _elementSize, uint32_t _w, uint32_t _h, uint32_t _d);
        virtual WNRenderTarget* CreateRenderTarget(WNTexture* _texture);
        virtual WNDrawList* CreateDrawList();

        // Resource Binding
        virtual WNGraphicsDeviceReturnCode::type BindShader(WNShader* _resource);
        virtual WNGraphicsDeviceReturnCode::type BindBuffer(WNBuffer* _resource, uint32_t _location);
        virtual WNGraphicsDeviceReturnCode::type BindTexture(WNTexture* _texture, uint32_t _location);
        virtual WNGraphicsDeviceReturnCode::type BindRenderTarget(WNRenderTarget* _texture, uint32_t _location);

        // State Setting
        virtual void SetClearColor(float* _color);
        virtual WNGraphicsDeviceReturnCode::type BindSurface(wn::memory::intrusive_ptr<wn::surface>& _surface, bool _sync);

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
            bool mSync;
        };

    private:
        wn::memory::intrusive_ptr<wn::surface> mActiveSurface;
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
