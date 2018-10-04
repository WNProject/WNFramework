// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_JIT_COMPILER_H__
#define __WN_SCRIPTING_JIT_COMPILER_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNHashSet.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace llvm {
class Module;
}  // namespace llvm

namespace wn {
namespace scripting {

namespace internal {
struct jit_compiler_context;
}

class jit_compiler {
public:
  jit_compiler(memory::allocator* _allocator, llvm::Module* _module,
    containers::hash_map<const ast_type*, struct_info>* _types);
  ~jit_compiler();

  bool compile(ast_script_file* _file);

  // We actually do our compilation in the context.
  // We have to do this so we dont pollute the universe with
  // llvm includes.

private:
  memory::unique_ptr<internal::jit_compiler_context> m_compiler_context;
};
}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_JIT_COMPILER_H__
