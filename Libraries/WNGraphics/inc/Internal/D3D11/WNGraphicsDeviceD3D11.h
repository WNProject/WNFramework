////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_GRAPHICS_D3D11_GRAPHICS_DEVICE_D3D11_H__
#define __WN_GRAPHICS_D3D11_GRAPHICS_DEVICE_D3D11_H__

#include "WNGraphics/inc/WNGraphicsDevice.h"
#include "WNConcurrency/inc/WNRecursiveMutex.h"
#include "WNPlatform/inc/WNSurface.h"

struct IDXGIFactory1;
struct IDXGIAdapter1;
struct IDXGIOutput;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;

namespace WNGraphics {
    class WNGraphicsDeviceD3D11 : public WNGraphicsDevice {
    public:
        WNGraphicsDeviceD3D11(WNGraphics::WNGraphicsResourceFactory* _factory);

        virtual WNGraphicsDeviceReturnCode::Type Initialize(WN_UINT32 _adapter, WN_UINT32 _output);
        virtual WN_VOID Release();

        virtual WNGraphicsDeviceReturnCode::Type SetActiveSurface(WNConcurrency::WNResourcePointer<WNPlatform::WNSurface> _surface);

        // State Querying
        virtual WN_UINT32 GetCapability(WNDeviceCaps _cap);
        
        // Resource Creation
        virtual WNShader* CreateShader(WNShaderTypes _type, WN_CHAR* _text);
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

        virtual WNGraphicsDeviceReturnCode::Type BindSurface(WNConcurrency::WNResourcePointer<WNPlatform::WNSurface>& _surface, WN_BOOL _sync = WN_TRUE);

        // Drawing
        virtual WNGraphicsDeviceReturnCode::Type SetDrawList(WNDrawList* _list);
        virtual WNGraphicsDeviceReturnCode::Type StartDraw();
        virtual WNGraphicsDeviceReturnCode::Type EndDraw();
        virtual WNGraphicsDeviceReturnCode::Type SubmitDrawList(WNDrawList* _list);
        virtual WNGraphicsDeviceReturnCode::Type Clear();

    private:
        enum WNInitializationState {
            eWNISD3D11DeviceInitialized,
            eWNISDXGIOutputInitialized,
            eWNISDXGIAdapterInitialized,
            eWNISDXGIFactoryInitialized,
            eWNISUninitialized,
            eWNISMax
        };

    private:
        struct WNWindowData {
            ID3D11RenderTargetView* mBackBufferView;
            IDXGISwapChain* mSwapChain;
            WN_INT32 mForceResize;
            WN_BOOL mRequestedFullScreen;
            WN_BOOL mFullScreen;
            WN_UINT32 mLastWindowedWidth;
            WN_UINT32 mLastWindowedHeight;
            WN_UINT32 mCurrentWidth;
            WN_UINT32 mCurrentHeight;
        };

    private:
        WN_VOID Cleanup();
        WN_BOOL ProcessWindowMessage(WNPlatform::WNSurface::WNSurfaceEventType _message, WNPlatform::WNSurface* _surface);
        WNGraphicsDeviceReturnCode::Type ResizeSwapChain();

    private:
        IDXGIFactory1* mFactory;
        IDXGIAdapter1* mAdapter;
        IDXGIOutput*   mOutput;
        ID3D11Device* mDevice;
        ID3D11DeviceContext* mContext;

        WNConcurrency::WNResourcePointer<WNPlatform::WNSurface> mActiveSurface;

        WNGraphicsResourceFactory * mResourceFactory;
        WNConcurrency::WNRecursiveMutex mResizeMutex;

        WN_FLOAT32 mClearColor[4];

        WNDrawList* mCurrentDrawList;

        WN_SIZE_T mMemory;
        WN_UINT32 mFeatureLevel;
        WN_UINT32 mScreenWidth;
        WN_UINT32 mScreenHeight;
        WN_UINT32 mScreenNumerator;
        WN_UINT32 mScreenDenominator;

        WN_BOOL mVSync;
        WN_BOOL mBackBufferBound;
        WNInitializationState mInitializationState;
    };
}

#endif // __WN_GRAPHICS_D3D11_GRAPHICS_DEVICE_D3D11_H__
