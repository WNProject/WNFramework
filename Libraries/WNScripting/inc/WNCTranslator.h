// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_C_TRANSLATOR_H__
#define __WN_SCRIPTING_C_TRANSLATOR_H__

#include "WNContainers/inc/WNString.h"
#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/WNTranslator.h"
#include "WNScripting/inc/type_manager.h"

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
  c_translator(memory::allocator* _allocator,
      file_system::mapping* _source_mapping,
      file_system::mapping* _dest_mapping, logging::log* _log);

  // Looks in m_file_manager for the path given by the file parameter,
  // and translates the file into C. The file is saved in the same location
  // as the c file with an additional .c extension.
  // If _dump_ast_on_failure is true, then the AST will be added to the log on
  // failure
  parse_error translate_file_with_error(
      const containers::string_view file, bool _dump_ast_on_failure) override;

private:
  ast_type* register_external_type(containers::string_view _name) override;
  struct external_function_data {
    containers::dynamic_array<type_classification> types;
    containers::string name;
  };

  file_system::mapping* m_source_mapping;
  file_system::mapping* m_dest_mapping;
  logging::log* m_compilation_log;
};
}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_C_TRANSLATOR_H__
