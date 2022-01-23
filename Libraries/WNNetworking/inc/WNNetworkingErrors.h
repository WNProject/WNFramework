// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NEWORKING_ERROR_H__
#define __WN_NEWORKING_ERROR_H__

namespace wn {
namespace networking {

enum class network_error {
#include "core/inc/internal/errors.inc"
  could_not_bind,
  could_not_resolve,
  could_not_send,
  could_not_receive,
  closed
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NEWORKING_ERROR_H__
