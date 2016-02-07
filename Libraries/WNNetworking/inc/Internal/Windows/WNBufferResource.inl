// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_INTERNAL_WINDOWS_BUFFER_RESOURCE_INL__
#define __WN_NETWORKING_INTERNAL_WINDOWS_BUFFER_RESOURCE_INL__

#include "WNCore/inc/WNBase.h"

namespace WNNetworking {
WN_FORCE_INLINE const WSABUF* WNBufferResource::GetWriteWinBuf() const {
  return (&mWinBuf);
}

WN_FORCE_INLINE WSABUF* WNBufferResource::GetReadWinBuf() {
  return (&mWinBuf);
}
}

#endif  // __WN_NETWORKING_INTERNAL_WINDOWS_BUFFER_RESOURCE_INL__