// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_C_TRANSLATOR_H__
#define __WN_SCRIPTING_C_TRANSLATOR_H__

#include "WNContainers/inc/WNString.h"
#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNTranslator.h"

namespace WNLogging {
class WNLogger;
}

namespace wn {
namespace memory {
class allocator;
}

namespace scripting {
class file_manager;
class c_translator : public translator {
 public:
  c_translator(memory::allocator* _allocator, file_manager* _manager,
               WNLogging::WNLog* _log);
  parse_error translate_file(const char* file) override;

 private:
  memory::allocator* m_allocator;
  file_manager* m_file_manager;
  WNLogging::WNLog* m_compilation_log;
};
}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_C_TRANSLATOR_H__
