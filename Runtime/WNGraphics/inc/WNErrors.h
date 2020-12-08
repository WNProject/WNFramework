// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_ERRORS_H__
#define __WN_GRAPHICS_ERRORS_H__

namespace wn {
namespace runtime {
namespace graphics {

enum class graphics_error {
#include "core/inc/internal/errors.inc"
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn
#endif  // __WN_GRAPHICS_ERRORS_H__
