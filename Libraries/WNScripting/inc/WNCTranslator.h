// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

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

namespace file_system {
class mapping;
}

namespace scripting {
// Create C files from the given scripting files.
class c_translator : public translator {
public:
  c_translator(type_validator* _validator, memory::allocator* _allocator,
      file_system::mapping* _mapping, WNLogging::WNLog* _log);

  // Looks in m_file_manager for the path given by the file parameter,
  // and translates the file into C. The file is saved in the same location
  // as the c file with an additional .c extension.
  parse_error translate_file(const char* file) override;

private:
  memory::allocator* m_allocator;
  file_system::mapping* m_file_mapping;
  WNLogging::WNLog* m_compilation_log;
};
}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_C_TRANSLATOR_H__
