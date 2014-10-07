////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

        virtual WNGraphicsDeviceReturnCode::Type Initialize(WN_UINT32 _adapter, WN_UINT32 _output);
        virtual WN_FORCE_INLINE WN_VOID Release() {}

        // State Querying
        virtual WN_UINT32 GetCapability(WNDeviceCaps _cap);
        virtual WNGraphicsDeviceReturnCode::Type GetSize(WN_UINT32 &_width, WN_UINT32 &_height);
        virtual WNGraphicsDeviceReturnCode::Type SetActiveSurface(WNConcurrency::WNResourcePointer<WNPlatform::WNSurface> _surface);

        // Resource Creation
        virtual WNShader* CreateShader(WNShaderTypes _type, WN_CHAR* _shaderText);
        virtual WNBuffer* CreateBuffer(WNBufferTypes _type, WN_UINT32 _elementSize, WN_UINT32 _w, WN_UINT32 _h, WN_UINT32 _d);
        virtual WNTexture* CreateTexture(WNTextureTypes _type, WNTextureFormat _format, WN_UINT32 _elementSize, WN_UINT32 _w, WN_UINT32 _h, WN_UINT32 _d);
        virtual WNRenderTarget* CreateRenderTarget(WNTexture* _texture);
        virtual WNDrawList* CreateDrawList();

        // Resource Binding
        virtual WNGraphicsDeviceReturnCode::Type BindShader(WNShader* _resource);
        virtual WNGraphicsDeviceReturnCode::Type BindBuffer(WNBuffer* _resource, WN_UINT32 _location);
        virtual WNGraphicsDeviceReturnCode::Type BindTexture(WNTexture* _texture, WN_UINT32 _location);
        virtual WNGraphicsDeviceReturnCode::Type BindRenderTarget(WNRenderTarget* _texture, WN_UINT32 _location);

        // State Setting
        virtual WN_VOID SetClearColor(WN_FLOAT32* _color);
        virtual WNGraphicsDeviceReturnCode::Type BindSurface(WNConcurrency::WNResourcePointer<WNPlatform::WNSurface>& _surface, WN_BOOL _sync);

        // Drawing
        virtual WNGraphicsDeviceReturnCode::Type SetDrawList(WNDrawList* _list);
        virtual WNGraphicsDeviceReturnCode::Type StartDraw();
        virtual WNGraphicsDeviceReturnCode::Type EndDraw();
        virtual WNGraphicsDeviceReturnCode::Type SubmitDrawList(WNDrawList* _list);
        virtual WNGraphicsDeviceReturnCode::Type Clear();
        


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
            WN_BOOL mSync;
        };

    private:
        WNConcurrency::WNResourcePointer<WNPlatform::WNSurface> mActiveSurface;
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
