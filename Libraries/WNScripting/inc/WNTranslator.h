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
  translator() : m_num_warnings(0), m_num_errors(0) {}
  virtual ~translator(){};
  virtual parse_error translate_file(const char* file) = 0;
  wn_size_t errors() const { return m_num_errors; }
  wn_size_t warnings() const { return m_num_warnings; }

 protected:
   wn_size_t m_num_warnings;
   wn_size_t m_num_errors;
};
}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_TRANSLATOR_H__
