// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_ERRORS_H__
#define __WN_GRAPHICS_ERRORS_H__

namespace wn {
namespace runtime {
namespace graphics {

enum class graphics_error {
#include "WNCore/inc/internal/errors.inc"
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn
#endif  // __WN_GRAPHICS_ERRORS_H__
