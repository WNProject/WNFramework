// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_AST_PASSES_H__
#define __WN_SCRIPTING_AST_PASSES_H__
#include "WNCore/inc/WNTypes.h"

namespace WNLogging {
class WNLog;
}

namespace wn {
namespace scripting {
class script_file;
class type_validator;

// Runs the type_association pass. This is required to associate
// a type with all nodes who'se type is inferred from the context.
// For example the type of a binary expression depends on the type
// of its operands.
// Returns false, and writes to the _log parameter if a type
// could not be determined.
bool run_type_association_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors);

// Associates all IDs with the parameters/declaration of the id.
// Returns false, and writes to the _log parameter if an id was not
// accessible in the current scope.
bool run_id_association_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors);

// Removes dead code from the tree. Examples include:
// Code after return.
// TODO(awoloszyn): Remove constant branches.
bool run_dce_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors);

// TODO(awoloszyn): Constant folding pass.

}  // namespace scripting
}  // namesapce wn
#endif  //__WN_SCRIPTING_AST_PASSES_H__
