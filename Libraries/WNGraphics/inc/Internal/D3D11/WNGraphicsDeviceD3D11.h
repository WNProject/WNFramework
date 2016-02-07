// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_D3D11_GRAPHICS_DEVICE_D3D11_H__
#define __WN_GRAPHICS_D3D11_GRAPHICS_DEVICE_D3D11_H__

#include "WNGraphics/inc/WNGraphicsDevice.h"
#include "WNMultiTasking/inc/WNMutex.h"
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

  virtual WNGraphicsDeviceReturnCode::type Initialize(
      uint32_t _adapter, uint32_t _output);
  virtual void Release();

  virtual WNGraphicsDeviceReturnCode::type SetActiveSurface(
      wn::memory::intrusive_ptr<wn::surface> _surface);

  // State Querying
  virtual uint32_t GetCapability(WNDeviceCaps _cap);

  // Resource Creation
  virtual WNShader* CreateShader(WNShaderTypes _type, char* _text);
  virtual WNBuffer* CreateBuffer(WNBufferTypes _type, uint32_t _elementSize,
      uint32_t _w, uint32_t _h, uint32_t _d);
  virtual WNTexture* CreateTexture(WNTextureTypes _type,
      WNTextureFormat _format, uint32_t _elementSize, uint32_t _w, uint32_t _h,
      uint32_t _d);
  virtual WNRenderTarget* CreateRenderTarget(WNTexture* _texture);
  virtual WNDrawList* CreateDrawList();

  // Resource Binding
  virtual WNGraphicsDeviceReturnCode::type BindShader(WNShader* _resource);
  virtual WNGraphicsDeviceReturnCode::type BindBuffer(
      WNBuffer* _resource, uint32_t _location);
  virtual WNGraphicsDeviceReturnCode::type BindTexture(
      WNTexture* _texture, uint32_t _location);
  virtual WNGraphicsDeviceReturnCode::type BindRenderTarget(
      WNRenderTarget* _texture, uint32_t _location);

  // State Setting
  virtual void SetClearColor(float* _color);

  virtual WNGraphicsDeviceReturnCode::type BindSurface(
      wn::memory::intrusive_ptr<wn::surface>& _surface, bool _sync = true);

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
    int32_t mForceResize;
    bool mRequestedFullScreen;
    bool mFullScreen;
    uint32_t mLastWindowedWidth;
    uint32_t mLastWindowedHeight;
    uint32_t mCurrentWidth;
    uint32_t mCurrentHeight;
  };

private:
  void Cleanup();
  bool ProcessWindowMessage(
      wn::surface::WNSurfaceEventType _message, wn::surface* _surface);
  WNGraphicsDeviceReturnCode::type ResizeSwapChain();

private:
  IDXGIFactory1* mFactory;
  IDXGIAdapter1* mAdapter;
  IDXGIOutput* mOutput;
  ID3D11Device* mDevice;
  ID3D11DeviceContext* mContext;

  wn::memory::intrusive_ptr<wn::surface> mActiveSurface;

  WNGraphicsResourceFactory* mResourceFactory;
  wn::multi_tasking::recursive_mutex mResizeMutex;

  float mClearColor[4];

  WNDrawList* mCurrentDrawList;

  size_t mMemory;
  uint32_t mFeatureLevel;
  uint32_t mScreenWidth;
  uint32_t mScreenHeight;
  uint32_t mScreenNumerator;
  uint32_t mScreenDenominator;

  bool mVSync;
  bool mBackBufferBound;
  WNInitializationState mInitializationState;
};
}

#endif  // __WN_GRAPHICS_D3D11_GRAPHICS_DEVICE_D3D11_H__
