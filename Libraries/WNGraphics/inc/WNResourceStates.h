// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_RESOURCE_STATES_H__
#define __WN_GRAPHICS_RESOURCE_STATES_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNMemory/inc/WNUniquePtr.h"

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/WNQueueIncludes.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

namespace wn {
namespace graphics {

enum class resource_state {
  initial,
  copy_source,
  copy_dest,
  render_target,
  max
};

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_RESOURCE_STATES_H__
