////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_GRAPHICS_GRAPHICS_DEVICE_H__
#define __WN_GRAPHICS_GRAPHICS_DEVICE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNConcurrency/inc/WNResourcePointer.h"

namespace WNPlatform {
    class WNSurface;
}

namespace WNGraphics {
    class WNGraphicsResourceFactory;
    class WNShader;
    class WNTexture;
    class WNRenderTarget;
    class WNBuffer;
    class WNBaseTexture;
    class WNDrawList;
    class WNGraphicsResource;

    enum WNDeviceCaps {
        eWNVertexShaderCaps,   //0, 3, 4, 5
        eWNPixelShaderCaps,    //0, 3, 4, 5
        eWNGeometryShaderCaps, //0, 4, 5
        eWNHullShaderCaps,     //0, 5
        eWNDomainShaderCaps,   //0, 5
        eWNComputeShaderCaps,  //0, 4, 5
        eWNMultiDrawDevice     //This indicates that with 2 devices you can have interleaved StartDraw() and EndDraw() calls
    };

    enum WNShaderTypes {
        eWNVertexShader,
        eWNPixelShader,
        eWNGeometryShader,
        eWNHullShader,
        eWNDomainShader,
        eWNComputeShader,
        eWNShaderEnd
    };

    enum WNBufferTypes {
        eWNVertexBuffer,
        eWNIndexBuffer,
        eWNConstantBuffer,
        eWNStructuredBuffer,
        eWNUnorderedBuffer,
        eWNAppendConsumeBuffer,
        eWNBufferEnd
    };

    enum WNTextureTypes {
        eWNTexture1D,
        eWNTexture2D,
        eWNTexture3D,
        eWNTextureCube,
        eWNTextureTypeEnd
    };

    enum WNTextureFormat {
        eWNRGBA8,
        eWNRGB8,
        eWNRenderTargetEnd,
        eWNBC1,
        eWNBC2,
        eWNBC3,
        eWNBC4,
        eWNBC5,
        eWNBC6,
        eWNBC7,
        eWNTextureEnd
    };

    namespace WNGraphicsDeviceReturnCode {
        enum Type {
            #include "WNCore/inc/Internal/WNErrors.inc"
            eWNGDEWindowCreationError,
            eWNGDEDisplayModeError,
            eWNGDEMonitorError,
            eWNGDEBackBufferError,
            eWNGDESwapChainCreationError,
            eWNGDETooManySurfaces,
            eWNGDEResizeFail,
            eWNGDESizeFail,
            eWNGDEBadSurface,
            eWNGDENoSurface,
            eWNGDERequiredSurface,
        };
    }

    class WNGraphicsDevice {
    public:
        virtual WNGraphicsDeviceReturnCode::Type Initialize(WN_UINT32 _adapter, WN_UINT32 _device) = 0;
        virtual WN_VOID Release() = 0;

        // Window
        virtual WNGraphicsDeviceReturnCode::Type SetActiveSurface(WNConcurrency::WNResourcePointer<WNPlatform::WNSurface> _surface) = 0;

        // State Querying
        virtual WN_UINT32 GetCapability(WNDeviceCaps _cap) = 0;

        // Resource Creation
        virtual WNShader* CreateShader(WNShaderTypes _type, WN_CHAR* _shaderText) = 0;
        virtual WNBuffer* CreateBuffer(WNBufferTypes _type, WN_UINT32 _elementSize, WN_UINT32 _w, WN_UINT32 _h, WN_UINT32 _d) = 0;
        virtual WNTexture* CreateTexture(WNTextureTypes _type, WNTextureFormat _format, WN_UINT32 _elementSize, WN_UINT32 _w, WN_UINT32 _h, WN_UINT32 _d) = 0;
        virtual WNRenderTarget* CreateRenderTarget(WNTexture* _texture) = 0;
        virtual WNDrawList* CreateDrawList() = 0;

        // Resource Binding
        virtual WNGraphicsDeviceReturnCode::Type BindShader(WNShader* _resource) = 0;
        virtual WNGraphicsDeviceReturnCode::Type BindBuffer(WNBuffer* _resource, WN_UINT32 _location) = 0;
        virtual WNGraphicsDeviceReturnCode::Type BindTexture(WNTexture* _texture, WN_UINT32 _location) = 0;
        virtual WNGraphicsDeviceReturnCode::Type BindRenderTarget(WNRenderTarget* _texture, WN_UINT32 _location) = 0;

        // State Setting
        virtual WN_VOID SetClearColor(WN_FLOAT32* _color) = 0;

        virtual WNGraphicsDeviceReturnCode::Type BindSurface(WNConcurrency::WNResourcePointer<WNPlatform::WNSurface>& _surface, WN_BOOL _sync = WN_TRUE) = 0;

        // Drawing
        virtual WNGraphicsDeviceReturnCode::Type SetDrawList(WNDrawList* _list) = 0;
        virtual WNGraphicsDeviceReturnCode::Type StartDraw() = 0;
        virtual WNGraphicsDeviceReturnCode::Type EndDraw() = 0;
        virtual WNGraphicsDeviceReturnCode::Type SubmitDrawList(WNDrawList* _list) = 0;
        virtual WNGraphicsDeviceReturnCode::Type Clear() = 0;
    };
}

#endif // __WN_GRAPHICS_GRAPHICS_DEVICE_H__
