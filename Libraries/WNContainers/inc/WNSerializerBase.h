// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_SERIALIZER_BASE_H__
#define __WN_CONTAINERS_SERIALIZER_BASE_H__

#include "WNCore/inc/types.h"
#include "WNCore/inc/utilities.h"

namespace wn {
namespace containers {
class data_buffer;

class serializer_base : public core::non_copyable {
public:
  virtual size_t serialize(
      data_buffer& _buffer, const uint32_t _flags) const = 0;
};
}
}

#endif  // __WN_CONTAINERS_SERIALIZER_BASE_H__