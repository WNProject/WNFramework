// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_TRANSLATOR_H__
#define __WN_SCRIPTING_TRANSLATOR_H__

#include "WNContainers/inc/WNString.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNEngineFileManager.h"
#include "WNScripting/inc/WNEngine.h"

namespace WNLogging {
class WNLogger;
}

namespace wn {
namespace scripting {

class translator {
 public:
  virtual ~translator(){};
  virtual parse_error translate_file(const char* file) = 0;

 private:
};
}  // namesace scripting
}  // namesapce wn

#endif  //__WN_SCRIPTING_TRANSLATOR_H__
