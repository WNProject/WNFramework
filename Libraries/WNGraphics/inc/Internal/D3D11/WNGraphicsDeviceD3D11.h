// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_D3D11_GRAPHICS_DEVICE_D3D11_H__
#define __WN_GRAPHICS_D3D11_GRAPHICS_DEVICE_D3D11_H__

#include "WNGraphics/inc/WNGraphicsDevice.h"
#include "WNThreading/inc/WNMutex.h"
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

        virtual WNGraphicsDeviceReturnCode::type Initialize(wn_uint32 _adapter, wn_uint32 _output);
        virtual wn_void Release();

        virtual WNGraphicsDeviceReturnCode::type SetActiveSurface(wn::memory::intrusive_ptr<wn::surface> _surface);

        // State Querying
        virtual wn_uint32 GetCapability(WNDeviceCaps _cap);

        // Resource Creation
        virtual WNShader* CreateShader(WNShaderTypes _type, wn_char* _text);
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

        virtual WNGraphicsDeviceReturnCode::type BindSurface(wn::memory::intrusive_ptr<wn::surface>& _surface, wn_bool _sync = wn_true);

        // Drawing
        virtual WNGraphicsDeviceReturnCode::type SetDrawList(WNDrawList* _list);
        virtual WNGraphicsDeviceReturnCode::type StartDraw();
        virtual WNGraphicsDeviceReturnCode::type EndDraw();
        virtual WNGraphicsDeviceReturnCode::type SubmitDrawList(WNDrawList* _list);
        virtual WNGraphicsDeviceReturnCode::type Clear();

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
            wn_int32 mForceResize;
            wn_bool mRequestedFullScreen;
            wn_bool mFullScreen;
            wn_uint32 mLastWindowedWidth;
            wn_uint32 mLastWindowedHeight;
            wn_uint32 mCurrentWidth;
            wn_uint32 mCurrentHeight;
        };

    private:
        wn_void Cleanup();
        wn_bool ProcessWindowMessage(wn::surface::WNSurfaceEventType _message, wn::surface* _surface);
        WNGraphicsDeviceReturnCode::type ResizeSwapChain();

    private:
        IDXGIFactory1* mFactory;
        IDXGIAdapter1* mAdapter;
        IDXGIOutput*   mOutput;
        ID3D11Device* mDevice;
        ID3D11DeviceContext* mContext;

        wn::memory::intrusive_ptr<wn::surface> mActiveSurface;

        WNGraphicsResourceFactory * mResourceFactory;
        wn::threading::recursive_mutex mResizeMutex;

        wn_float32 mClearColor[4];

        WNDrawList* mCurrentDrawList;

        wn_size_t mMemory;
        wn_uint32 mFeatureLevel;
        wn_uint32 mScreenWidth;
        wn_uint32 mScreenHeight;
        wn_uint32 mScreenNumerator;
        wn_uint32 mScreenDenominator;

        wn_bool mVSync;
        wn_bool mBackBufferBound;
        WNInitializationState mInitializationState;
    };
}

#endif // __WN_GRAPHICS_D3D11_GRAPHICS_DEVICE_D3D11_H__
