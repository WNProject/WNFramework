// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_WINDOW_WINDOW_ERROR_H__
#define __WN_WINDOW_WINDOW_ERROR_H__

namespace wn {
namespace runtime {
namespace window {

enum class window_error {
#include "WNCore/inc/internal/errors.inc"
};

}  // namespace window
}  // namespace runtime
}  // namespace wn

#endif  // __WN_WINDOW_WINDOW_ERROR_H__