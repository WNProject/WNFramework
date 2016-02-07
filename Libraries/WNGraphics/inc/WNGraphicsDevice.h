// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_GRAPHICS_DEVICE_H__
#define __WN_GRAPHICS_GRAPHICS_DEVICE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNPlatform/inc/WNSurface.h"

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
  eWNVertexShaderCaps,    // 0, 3, 4, 5
  eWNPixelShaderCaps,     // 0, 3, 4, 5
  eWNGeometryShaderCaps,  // 0, 4, 5
  eWNHullShaderCaps,      // 0, 5
  eWNDomainShaderCaps,    // 0, 5
  eWNComputeShaderCaps,   // 0, 4, 5
  eWNMultiDrawDevice      // This indicates that with 2 devices you can have
                      // interleaved StartDraw() and EndDraw() calls
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
enum type {
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
  virtual WNGraphicsDeviceReturnCode::type Initialize(
      uint32_t _adapter, uint32_t _device) = 0;
  virtual void Release() = 0;

  // Window
  virtual WNGraphicsDeviceReturnCode::type SetActiveSurface(
      wn::memory::intrusive_ptr<wn::surface> _surface) = 0;

  // State Querying
  virtual uint32_t GetCapability(WNDeviceCaps _cap) = 0;

  // Resource Creation
  virtual WNShader* CreateShader(WNShaderTypes _type, char* _shaderText) = 0;
  virtual WNBuffer* CreateBuffer(WNBufferTypes _type, uint32_t _elementSize,
      uint32_t _w, uint32_t _h, uint32_t _d) = 0;
  virtual WNTexture* CreateTexture(WNTextureTypes _type,
      WNTextureFormat _format, uint32_t _elementSize, uint32_t _w, uint32_t _h,
      uint32_t _d) = 0;
  virtual WNRenderTarget* CreateRenderTarget(WNTexture* _texture) = 0;
  virtual WNDrawList* CreateDrawList() = 0;

  // Resource Binding
  virtual WNGraphicsDeviceReturnCode::type BindShader(WNShader* _resource) = 0;
  virtual WNGraphicsDeviceReturnCode::type BindBuffer(
      WNBuffer* _resource, uint32_t _location) = 0;
  virtual WNGraphicsDeviceReturnCode::type BindTexture(
      WNTexture* _texture, uint32_t _location) = 0;
  virtual WNGraphicsDeviceReturnCode::type BindRenderTarget(
      WNRenderTarget* _texture, uint32_t _location) = 0;

  // State Setting
  virtual void SetClearColor(float* _color) = 0;

  virtual WNGraphicsDeviceReturnCode::type BindSurface(
      wn::memory::intrusive_ptr<wn::surface>& _surface, bool _sync = true) = 0;

  // Drawing
  virtual WNGraphicsDeviceReturnCode::type SetDrawList(WNDrawList* _list) = 0;
  virtual WNGraphicsDeviceReturnCode::type StartDraw() = 0;
  virtual WNGraphicsDeviceReturnCode::type EndDraw() = 0;
  virtual WNGraphicsDeviceReturnCode::type SubmitDrawList(
      WNDrawList* _list) = 0;
  virtual WNGraphicsDeviceReturnCode::type Clear() = 0;
};
}

#endif  // __WN_GRAPHICS_GRAPHICS_DEVICE_H__
