// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/Internal/D3D11/WNGraphicsDeviceD3D11.h"
#include "WNPlatform/inc/WNSurface.h"
#include "WNMemory/inc/WNManipulation.h"

#include <mutex>

#include "DXGI.h"
#include "D3D11.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#define WN_RESIZE_RETRY_COUNT 5

WNGraphics::WNGraphicsDeviceD3D11::WNGraphicsDeviceD3D11(WNGraphics::WNGraphicsResourceFactory* _factory) :
    mResourceFactory(_factory),
    mFactory(NULL),
    mAdapter(NULL),
    mOutput(NULL),
    mDevice(NULL),
    mContext(NULL),
    mCurrentDrawList(NULL),
    mMemory(0),
    mFeatureLevel(0),
    mScreenWidth(0),
    mScreenHeight(0),
    mScreenNumerator(0),
    mScreenDenominator(0),
    mVSync(wn_false),
    mBackBufferBound(wn_false),
    mInitializationState(eWNISUninitialized) {
    mClearColor[0] = 0.0f;
    mClearColor[1] = 0.0f;
    mClearColor[2] = 0.0f;
    mClearColor[3] = 1.0f;
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceD3D11::Initialize(wn_uint32 _adapter, wn_uint32 _output) {
    if (CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&mFactory)) != S_OK) {
        Cleanup();
    }

    mInitializationState = WNGraphics::WNGraphicsDeviceD3D11::eWNISDXGIFactoryInitialized;

    if (mFactory->EnumAdapters1(_adapter, &mAdapter) != S_OK) {
        Cleanup();
    }

    mInitializationState = WNGraphics::WNGraphicsDeviceD3D11::eWNISDXGIAdapterInitialized;

    if (mAdapter->EnumOutputs(_output, &mOutput) != S_OK) {
        Cleanup();
    }

    mInitializationState = WNGraphics::WNGraphicsDeviceD3D11::eWNISDXGIOutputInitialized;

    D3D_FEATURE_LEVEL level;
    UINT flags = 0;

    #ifdef _WN_DEBUG
        flags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    if (D3D11CreateDevice(mAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, flags, NULL, 0, D3D11_SDK_VERSION, &mDevice, &level, &mContext) != S_OK) {
        if (D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL, flags, NULL, 0, D3D11_SDK_VERSION, &mDevice, &level, &mContext) != S_OK) {
            //Try to fallback on a software WARP device
            Cleanup();
        }
    }

    mFeatureLevel = static_cast<wn_uint32>(level);
    mInitializationState = WNGraphics::WNGraphicsDeviceD3D11::eWNISD3D11DeviceInitialized;

    DXGI_ADAPTER_DESC1 adapterDesc = {0};

    mAdapter->GetDesc1(&adapterDesc);
    mMemory = adapterDesc.DedicatedVideoMemory;

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

wn_void WNGraphics::WNGraphicsDeviceD3D11::Release() {
    Cleanup();
}

wn_void WNGraphics::WNGraphicsDeviceD3D11::Cleanup() {
    switch(mInitializationState) {
    case WNGraphics::WNGraphicsDeviceD3D11::eWNISD3D11DeviceInitialized:
        mDevice->Release();
        mDevice = wn_nullptr;
        mContext->Release();
        mContext = wn_nullptr;
    case WNGraphics::WNGraphicsDeviceD3D11::eWNISDXGIOutputInitialized:
        mOutput->Release();
        mOutput = wn_nullptr;
    case WNGraphics::WNGraphicsDeviceD3D11::eWNISDXGIAdapterInitialized:
        mAdapter->Release();
        mAdapter = wn_nullptr;
    case WNGraphics::WNGraphicsDeviceD3D11::eWNISDXGIFactoryInitialized:
        mFactory->Release();
        mFactory = wn_nullptr;
    case WNGraphics::WNGraphicsDeviceD3D11::eWNISUninitialized:
        break;
    };

    mInitializationState = eWNISUninitialized;
}

wn_uint32 WNGraphics::WNGraphicsDeviceD3D11::GetCapability(WNGraphics::WNDeviceCaps _cap) {
    switch(_cap) {
        case WNGraphics::eWNMultiDrawDevice:
            return(1);
    }

    return(0);
}

WNGraphics::WNShader* WNGraphics::WNGraphicsDeviceD3D11::CreateShader(WNGraphics::WNShaderTypes _type, wn_char* _shaderText) {
    WN_UNUSED_ARGUMENT(_type);
    WN_UNUSED_ARGUMENT(_shaderText);

    return(wn_nullptr);
}

WNGraphics::WNBuffer* WNGraphics::WNGraphicsDeviceD3D11::CreateBuffer(WNGraphics::WNBufferTypes _type, wn_uint32 _elementSize, wn_uint32 _w, wn_uint32 _h, wn_uint32 _d) {
    WN_UNUSED_ARGUMENT(_type);
    WN_UNUSED_ARGUMENT(_elementSize);
    WN_UNUSED_ARGUMENT(_w);
    WN_UNUSED_ARGUMENT(_h);
    WN_UNUSED_ARGUMENT(_d);

    return(wn_nullptr);
}

WNGraphics::WNTexture* WNGraphics::WNGraphicsDeviceD3D11::CreateTexture(WNGraphics::WNTextureTypes _type, WNGraphics::WNTextureFormat _format, wn_uint32 _elementSize, wn_uint32 _w, wn_uint32 _h, wn_uint32 _d) {
    WN_UNUSED_ARGUMENT(_type);
    WN_UNUSED_ARGUMENT(_format);
    WN_UNUSED_ARGUMENT(_elementSize);
    WN_UNUSED_ARGUMENT(_w);
    WN_UNUSED_ARGUMENT(_h);
    WN_UNUSED_ARGUMENT(_d);

    return(wn_nullptr);
}

WNGraphics::WNRenderTarget* WNGraphics::WNGraphicsDeviceD3D11::CreateRenderTarget(WNGraphics::WNTexture* _texture) {
    WN_UNUSED_ARGUMENT(_texture);

    return(wn_nullptr);
}

WNGraphics::WNDrawList* WNGraphics::WNGraphicsDeviceD3D11::CreateDrawList() {
    return(wn_nullptr);
}

wn_void WNGraphics::WNGraphicsDeviceD3D11::SetClearColor(wn_float32* _color) {
    wn::memory::memory_copy(mClearColor, _color, 4);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceD3D11::BindSurface(wn::memory::intrusive_ptr<wn::surface>& _surface, wn_bool _sync) {
    HWND handle = _surface->GetNativeHandle();
    UINT numModes = 0;
    DXGI_OUTPUT_DESC outputDesc = {0};

    if (mOutput->GetDesc(&outputDesc) != S_OK) {
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEDisplayModeError);
    }

    MONITORINFO monitorInfo = {0};

    monitorInfo.cbSize = sizeof(MONITORINFO);

    if (!GetMonitorInfo(outputDesc.Monitor, &monitorInfo)) {
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEMonitorError);
    }

    const wn_uint32 width = static_cast<wn_uint32>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);
    const wn_uint32 height = static_cast<wn_uint32>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);
    const HRESULT hr = mOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);

    if (S_OK == hr) {
        if (mOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL) != S_OK) {
            return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEDisplayModeError);
        }

        DXGI_MODE_DESC* modeDesc = static_cast<DXGI_MODE_DESC*>(WN_STACK_ALLOC(sizeof(DXGI_MODE_DESC) * numModes));

        if (mOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, modeDesc) != S_OK) {
            return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEDisplayModeError);
        }

        mScreenNumerator = mScreenDenominator = 0;

        for (wn_uint32 i = 0; i < static_cast<wn_uint32>(numModes); ++i) {
            if (modeDesc[i].Width == static_cast<UINT>(width) && modeDesc[i].Height == static_cast<UINT>(height)) {
                mScreenNumerator = modeDesc[i].RefreshRate.Numerator;
                mScreenDenominator = modeDesc[i].RefreshRate.Denominator;
                mScreenWidth = width;
                mScreenHeight = height;
                break;
            }
        }

        if (mScreenNumerator == 0 || mScreenDenominator == 0) {
            return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEDisplayModeError);
        }
    } else if (DXGI_ERROR_NOT_CURRENTLY_AVAILABLE == hr) {
        //We are remote desktop (this is most likely for testing, if you are using this for REAL .. I make no promises
        mScreenWidth = width;
        mScreenHeight = height;
        mScreenNumerator = 1;
        mScreenDenominator = 30;
    } else {
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEDisplayModeError);
    }

    mVSync = _sync;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {0};

    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = _surface->GetWidth();
    swapChainDesc.BufferDesc.Height = _surface->GetHeight();
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = mScreenNumerator;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = mScreenDenominator;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = handle;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    WNWindowData* dat = wn::memory::heap_allocate<WNWindowData>();

    wn::memory::memory_zero(dat);

    if (mFactory->CreateSwapChain(mDevice, &swapChainDesc, &dat->mSwapChain) != S_OK) {
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDESwapChainCreationError);
    }

    if (mFactory->MakeWindowAssociation(handle, DXGI_MWA_NO_ALT_ENTER) != S_OK) {
        dat->mSwapChain->Release();
        dat->mSwapChain = wn_nullptr;

        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDESwapChainCreationError);
    }

    if (dat->mSwapChain->ResizeBuffers(0, _surface->GetWidth(), _surface->GetHeight(), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH) != S_OK) {
        dat->mSwapChain->Release();
        dat->mSwapChain = wn_nullptr;

        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDESwapChainCreationError);
    }

    dat->mCurrentWidth = _surface->GetWidth();
    dat->mCurrentHeight = _surface->GetHeight();
    dat->mLastWindowedWidth = _surface->GetWidth();
    dat->mLastWindowedHeight = _surface->GetHeight();

    ID3D11Texture2D* backBufferTexture;

    if (dat->mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture) != S_OK) {
        dat->mSwapChain->Release();
        dat->mSwapChain = wn_nullptr;

        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEBackBufferError);
    }

    if (mDevice->CreateRenderTargetView(backBufferTexture, NULL, &dat->mBackBufferView) != S_OK) {
        backBufferTexture->Release();
        dat->mSwapChain->Release();
        dat->mSwapChain = wn_nullptr;

        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEBackBufferError);
    }

    _surface->RegisterData(wn::surface::eWNRDTGraphics, dat);

    backBufferTexture->Release();
    _surface->RegisterCallback(wn::surface::eWNSETResize, wn::surface::WNSurfaceEventCallback(&WNGraphicsDeviceD3D11::ProcessWindowMessage, this));
    _surface->RegisterCallback(wn::surface::eWNSETDestroyed, wn::surface::WNSurfaceEventCallback(&WNGraphicsDeviceD3D11::ProcessWindowMessage, this));

    BindRenderTarget(wn_nullptr, 0);

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceD3D11::SetDrawList(WNGraphics::WNDrawList* _list) {
    WN_UNUSED_ARGUMENT(_list);

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceD3D11::StartDraw() {
    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceD3D11::EndDraw() {
    if (!mActiveSurface) {
        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDENoSurface);
    }

    WNWindowData* dat = mActiveSurface->GetRegisteredData<WNWindowData>(wn::surface::eWNRDTGraphics);

    if (!dat->mSwapChain) {
        return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
    }

    if (dat->mForceResize > 0) {
        ResizeSwapChain();

        if (dat->mForceResize == 0) {
            return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEResizeFail);
        }
    }

    if (mCurrentDrawList == wn_nullptr) {
        if (mVSync) {
            if (dat->mSwapChain->Present(1, 0) != S_OK) {
                dat->mForceResize = WN_RESIZE_RETRY_COUNT;
            }
        } else {
            if (dat->mSwapChain->Present(0, 0) != S_OK) {
                dat->mForceResize = WN_RESIZE_RETRY_COUNT;
            }
        }
    }

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceD3D11::SubmitDrawList(WNGraphics::WNDrawList* _list) {
    WN_UNUSED_ARGUMENT(_list);

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

wn_bool WNGraphics::WNGraphicsDeviceD3D11::ProcessWindowMessage(wn::surface::WNSurfaceEventType _message, wn::surface* _surface) {
    if (_message == wn::surface::eWNSETResize) {
        std::lock_guard<wn::recursive_mutex> mScopeLock(mResizeMutex);
        WNWindowData* dat = _surface->GetRegisteredData<WNWindowData>(wn::surface::eWNRDTGraphics);

        dat->mForceResize = WN_RESIZE_RETRY_COUNT;
    }

    return(0);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceD3D11::BindShader(WNGraphics::WNShader* _resource) {
    WN_UNUSED_ARGUMENT(_resource);

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceD3D11::BindBuffer(WNGraphics::WNBuffer* _resource, wn_uint32 _location) {
    WN_UNUSED_ARGUMENT(_resource);
    WN_UNUSED_ARGUMENT(_location);

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceD3D11::BindTexture(WNGraphics::WNTexture* _texture, wn_uint32 _location) {
    WN_UNUSED_ARGUMENT(_texture);
    WN_UNUSED_ARGUMENT(_location);

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceD3D11::SetActiveSurface(wn::memory::intrusive_ptr<wn::surface> _surface) {
    mActiveSurface = _surface;

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceD3D11::BindRenderTarget(WNGraphics::WNRenderTarget* _texture, wn_uint32 _location) {
    if (_texture == wn_nullptr && _location == 0 && mActiveSurface) {
        WNWindowData* dat = mActiveSurface->GetRegisteredData<WNWindowData>(wn::surface::eWNRDTGraphics);

        mContext->OMSetRenderTargets(1, &dat->mBackBufferView, NULL);
        mBackBufferBound = wn_true;
    } else if (_location == 0) {
        mBackBufferBound = wn_false;
    }

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceD3D11::Clear() {
    if (mActiveSurface && mActiveSurface->GetRegisteredData<WNWindowData>(wn::surface::eWNRDTGraphics)->mBackBufferView) {
        mContext->ClearRenderTargetView(mActiveSurface->GetRegisteredData<WNWindowData>(wn::surface::eWNRDTGraphics)->mBackBufferView, mClearColor);
    }

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}

WNGraphics::WNGraphicsDeviceReturnCode::type WNGraphics::WNGraphicsDeviceD3D11::ResizeSwapChain() {
    WN_DEBUG_ASSERT(mActiveSurface);

    WNWindowData* dat = mActiveSurface->GetRegisteredData<WNWindowData>(wn::surface::eWNRDTGraphics);

    dat->mCurrentHeight = mActiveSurface->GetHeight();
    dat->mCurrentWidth = mActiveSurface->GetWidth();

    if (dat->mRequestedFullScreen != dat->mFullScreen) {
        if (dat->mRequestedFullScreen) {
            dat->mLastWindowedWidth = dat->mCurrentWidth;
            dat->mLastWindowedHeight = dat->mCurrentHeight;
            dat->mCurrentWidth = mScreenWidth;
            dat->mCurrentHeight = mScreenHeight;
        } else {
            dat->mCurrentWidth = dat->mLastWindowedWidth;
            dat->mCurrentHeight = dat->mLastWindowedHeight;
        }

        DXGI_MODE_DESC desc = {0};

        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.Width = dat->mCurrentWidth;
        desc.Height = dat->mCurrentHeight;
        desc.RefreshRate.Numerator = mScreenNumerator;
        desc.RefreshRate.Denominator = mScreenDenominator;
        desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

        dat->mFullScreen = dat->mRequestedFullScreen;

        if (dat->mSwapChain->SetFullscreenState(dat->mRequestedFullScreen, mOutput) != S_OK) {
            dat->mFullScreen = !dat->mFullScreen;
            dat->mForceResize--;

            return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEResizeFail);
        }

        mOutput->Release();
        mOutput = wn_nullptr;

        if (dat->mSwapChain->GetContainingOutput(&mOutput) != S_OK) {
            dat->mFullScreen = !dat->mFullScreen;
            dat->mForceResize--;

            return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEResizeFail);
        }

        desc.RefreshRate.Numerator = 0;
        desc.RefreshRate.Denominator = 0;

        if (dat->mSwapChain->ResizeTarget(&desc) != S_OK) {
            dat->mFullScreen = !dat->mFullScreen;
            dat->mForceResize--;

            return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEResizeFail);
        }

        return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
    }

    std::lock_guard<wn::recursive_mutex> scopeLock(mResizeMutex);

    if (dat->mFullScreen) {
        dat->mCurrentWidth = mScreenWidth;
        dat->mCurrentHeight = mScreenHeight;
    }

    if (mBackBufferBound) {
        ID3D11RenderTargetView* view = NULL;

        mContext->OMSetRenderTargets(1, &view, NULL);
    }

    dat->mBackBufferView->Release();
    dat->mBackBufferView = 0;

    if (dat->mSwapChain->ResizeBuffers(1, dat->mCurrentWidth, dat->mCurrentHeight, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH) != S_OK) {
        dat->mForceResize--;

        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEResizeFail);
    }

    ID3D11Texture2D* backBufferTexture;

    if (dat->mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture) != S_OK) {
        dat->mForceResize--;

        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEResizeFail);
    }

    if (mDevice->CreateRenderTargetView(backBufferTexture, NULL, &dat->mBackBufferView) != S_OK) {
        dat->mForceResize--;
        backBufferTexture->Release();

        return(WNGraphics::WNGraphicsDeviceReturnCode::eWNGDEResizeFail);
    }

    backBufferTexture->Release();

    if (mBackBufferBound) {
        mContext->OMSetRenderTargets(1, &dat->mBackBufferView, NULL);
    }

    dat->mForceResize = -1;

    return(WNGraphics::WNGraphicsDeviceReturnCode::ok);
}
