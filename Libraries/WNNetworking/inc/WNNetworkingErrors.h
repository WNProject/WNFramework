// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NEWORKING_ERROR_H__
#define __WN_NEWORKING_ERROR_H__

namespace wn {
namespace networking {

enum class network_error {
#include "WNCore/inc/Internal/WNErrors.inc"
  could_not_bind,
  could_not_resolve,
  could_not_send,
  could_not_receive,
  closed
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NEWORKING_ERROR_H__