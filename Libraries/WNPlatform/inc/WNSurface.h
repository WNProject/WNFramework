// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PLATFORM_SURFACE_H__
#define __WN_PLATFORM_SURFACE_H__

#include "WNContainers/inc/WNFunction.h"
#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

#ifdef _WN_LINUX
#include <X11/Xlib.h>
#endif
#ifdef _WN_ANDROID
#include <EGL/egl.h>
#endif

#ifdef _WN_MSVC
#pragma warning(push)
#pragma warning(disable : 4275)
#endif

#include <list>

#ifdef _WN_MSVC
#pragma warning(pop)
#endif

namespace wn {
#ifdef _WN_WINDOWS
typedef HWND WNSurfaceNativeHandle;
#elif defined _WN_LINUX
typedef Window WNSurfaceNativeHandle;
#elif defined _WN_ANDROID
typedef EGLSurface WNSurfaceNativeHandle;
#endif

class surface : public memory::intrusive_ptr_base {
public:
  surface() : memory::intrusive_ptr_base() {
    for (size_t i = 0; i < eWNRDTMAX; ++i) {
      mRegisteredTypes[i] = nullptr;
    }
  }

  virtual ~surface() {
    FireCallback(eWNSETDestroyed);
  }

  enum WNSurfaceError {
#include "WNCore/inc/Internal/WNErrors.inc"
    eWNSEStaticSize,
    eWNSETooLarge,
    eWNSEStaticPlacement,
    eWNSEInvalidPosition,
    eWNSEFullscreenOnly,
    eWNSEMAX
  };

  enum WNSurfaceEventType {
    eWNSETResize,
    eWNSETFullscreen,
    eWNSETDestroyed,
    eWNSETRecreated,
    eWNSETMAX
  };

  enum WNRegisteredDataTypes { eWNRDTGraphics, eWNRDTMAX };

  typedef wn::containers::function<bool(WNSurfaceEventType, surface*)>
      WNSurfaceEventCallback;

  virtual WNSurfaceNativeHandle GetNativeHandle() const = 0;
  virtual WNSurfaceError Resize(uint32_t _width, uint32_t _height) = 0;
  virtual WNSurfaceError Move(uint32_t _x, uint32_t _y) = 0;
  virtual bool IsFullscreen() const = 0;
  virtual WNSurfaceError SetFullscreen(bool _fullscreen) = 0;
  virtual uint32_t GetWidth() const = 0;
  virtual uint32_t GetHeight() const = 0;
  virtual uint32_t GetX() const = 0;
  virtual uint32_t GetY() const = 0;

  void RegisterCallback(
      WNSurfaceEventType _type, const WNSurfaceEventCallback& _callback) {
    mEventCallbacks[_type].push_back(_callback);
  }

  void RegisterData(WNRegisteredDataTypes _type, void* _data) {
    WN_RELEASE_ASSERT(_type < eWNRDTMAX);
    WN_RELEASE_ASSERT(mRegisteredTypes[_type] == nullptr);

    mRegisteredTypes[_type] = _data;
  }

  template <typename type>
  type* GetRegisteredData(const WNRegisteredDataTypes _type) {
    WN_RELEASE_ASSERT(_type < eWNRDTMAX);

    type* t = reinterpret_cast<type*>(mRegisteredTypes[_type]);

    return (t);
  }

protected:
  void FireCallback(WNSurfaceEventType _type) {
    for (std::list<WNSurfaceEventCallback>::iterator i =
             mEventCallbacks[_type].begin();
         i != mEventCallbacks[_type].end(); ++i) {
      (*i)(_type, this);
    }
  }

private:
  std::list<WNSurfaceEventCallback> mEventCallbacks[eWNSETMAX];
  void* mRegisteredTypes[eWNRDTMAX];
};

typedef memory::intrusive_ptr<surface> surface_handle;
}
#endif  // __WN_PLATFORM_SURFACE_H___
