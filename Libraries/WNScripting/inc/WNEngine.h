// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SCRIPTING_ENGINE_H__
#define __WN_SCRIPTING_ENGINE_H__

#include "WNScripting/inc/WNErrors.h"

namespace wn {
namespace scripting {

class engine {
public:
  virtual ~engine() {}
  virtual parse_error parse_file(const char* file) = 0;
};

}
}

#endif //__WN_SCRIPTING_ENGINE_H__